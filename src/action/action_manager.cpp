#include "action_manager.hpp"
#include "action/main.hpp"
#include "action/representable.hpp"
#include "action/setting.hpp"
#include "util/time.hpp"
#include <magic_enum.hpp>
#include <sstream>

namespace VTX
{
	namespace Action
	{
		void ActionManager::execute( BaseAction * const p_action, const bool p_flush )
		{
			if ( p_flush )
			{
				_flushAction( p_action );
			}
			else
			{
#ifdef DELAY_ACTIONS
				_actionQueue.emplace( p_action );
#else
				_flushAction( p_action );
#endif
			}
		}

		void ActionManager::execute( const std::string & p_action, const bool p_flush )
		{
			std::istringstream		 iss( p_action );
			std::string				 word;
			std::vector<std::string> words = std::vector<std::string>();
			while ( iss >> word )
			{
				words.emplace_back( word );
			}

			if ( words.size() == 0 )
			{
				VTX_ERROR( "Empty action string" );
			}

			std::string & command = words[ 0 ];
			BaseAction *  action  = nullptr;

			// TODO: map with ids.
			try
			{
				Model::Molecule & molecule = *( *VTXApp::get().getScene().getMolecules().begin() ).first;

				if ( command == "snapshot" )
				{
					action = new Main::Snapshot(
						Worker::Snapshoter::MODE::GL,
						Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) );
				}
				else if ( command == "change_representation" )
				{
					action = new Setting::ChangeRepresentation(
						magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "change_auto_rotate_speed" )
				{
					if ( words.size() == 2 )
					{
						action = new Setting::ChangeAutoRotateSpeed( Vec3f(
							std::stof( words.at( 1 ) ), std::stof( words.at( 1 ) ), std::stof( words.at( 1 ) ) ) );
					}
					else
					{
						action = new Setting::ChangeAutoRotateSpeed( Vec3f(
							std::stof( words.at( 1 ) ), std::stof( words.at( 2 ) ), std::stof( words.at( 3 ) ) ) );
					}
				}
				else if ( command == "change_shading" )
				{
					action = new Setting::ChangeShading(
						magic_enum::enum_cast<Renderer::SHADING>( words.at( 1 ) ).value() );
				}
				else if ( command == "change_color_mode" )
				{
					action = new Setting::ChangeColorMode(
						magic_enum::enum_cast<Generic::COLOR_MODE>( words.at( 1 ) ).value() );
				}
				else if ( command == "add_representation_molecule" )
				{
					action = new RepresentableAddRepresentation(
						molecule, molecule, magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "remove_representation_molecule" )
				{
					action = new RepresentableRemoveRepresentation(
						molecule, molecule, magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "add_representation_chain" )
				{
					action = new RepresentableAddRepresentation(
						*( *VTXApp::get().getScene().getMolecules().begin() )
							 .first->getChains()[ std::stoi( words.at( 2 ) ) ],
						molecule,
						magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "remove_representation_chain" )
				{
					action = new RepresentableRemoveRepresentation(
						*( *VTXApp::get().getScene().getMolecules().begin() )
							 .first->getChains()[ std::stoi( words.at( 2 ) ) ],
						molecule,
						magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "add_representation_residue" )
				{
					action = new RepresentableAddRepresentation(
						*( *VTXApp::get().getScene().getMolecules().begin() )
							 .first->getResidues()[ std::stoi( words.at( 2 ) ) ],
						molecule,
						magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
				else if ( command == "remove_representation_residue" )
				{
					action = new RepresentableRemoveRepresentation(
						*( *VTXApp::get().getScene().getMolecules().begin() )
							 .first->getResidues()[ std::stoi( words.at( 2 ) ) ],
						molecule,
						magic_enum::enum_cast<Generic::REPRESENTATION>( words.at( 1 ) ).value() );
				}
			}
			catch ( const std::exception & )
			{
				if ( action != nullptr )
				{
					action->displayUsage();
					delete action;
					return;
				}
			}

			if ( action != nullptr )
			{
				execute( action, p_flush );
			}
			else
			{
				VTX_ERROR( "Action not found" );
			}
		}

		bool ActionManager::canUndo() const { return _bufferUndo.size() > 0; }

		void ActionManager::undo()
		{
			if ( canUndo() == false )
			{
				return;
			}

			VTX_DEBUG( "Undo (" + std::to_string( _bufferUndo.size() - 1 ) + " more)" );
			_bufferUndo.front()->undo();
			_bufferRedo.push_front( _bufferUndo.front() );
			_bufferUndo.pop_front();
		}

		bool ActionManager::canRedo() const { return _bufferRedo.size() > 0; }

		void ActionManager::redo()
		{
			if ( canRedo() == false )
			{
				return;
			}

			VTX_DEBUG( "Redo (" + std::to_string( _bufferRedo.size() - 1 ) + " more)" );
			_bufferRedo.front()->redo();
			_bufferUndo.push_front( _bufferRedo.front() );
			_bufferRedo.pop_front();
		}

		void ActionManager::update( const double & p_deltaTime )
		{
			while ( _actionQueue.empty() == false )
			{
				_flushAction( _actionQueue.front() );
				_actionQueue.pop();
			}
		}

		void ActionManager::_flushAction( BaseAction * p_action )
		{
			try
			{
				p_action->execute();
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( p_e.what() );
			}

			// Handle undo.
			BaseActionUndonable * undonable = dynamic_cast<BaseActionUndonable *>( p_action );
			if ( undonable != nullptr )
			{
				_bufferUndo.push_front( undonable );
				_purgeBuffer();

				// TOCHECK: clear all redo actions? Only when same action? Always?
				// Clear redo actions.
				for ( BaseActionUndonable * action : _bufferRedo )
				{
					delete action;
				}
				_bufferRedo.clear();
			}
			else
			{
				delete p_action;
			}
		}

		void ActionManager::_purgeBuffer()
		{
			while ( _bufferUndo.size() > VTX::Setting::ACTION_BUFFER_SIZE )
			{
				delete _bufferUndo.back();
				_bufferUndo.pop_back();
			}
		}
	} // namespace Action
} // namespace VTX
