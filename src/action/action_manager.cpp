#include "action_manager.hpp"
#include "change_auto_rotate_speed.hpp"
#include "change_color_mode.hpp"
#include "change_representation.hpp"
#include "change_shading.hpp"
#include "snapshot.hpp"
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
				_actionQueue.push( p_action );
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
			if ( command == "snapshot" )
			{
				action = new Snapshot( Worker::Snapshoter::MODE::GL,
									   Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) );
			}
			else if ( command == "change_representation" )
			{
				action = new ChangeRepresentation();
			}
			else if ( command == "change_auto_rotate_speed" )
			{
				action = new ChangeAutoRotateSpeed();
			}
			else if ( command == "change_shading" )
			{
				action = new ChangeShading();
			}
			else if ( command == "change_color_mode" )
			{
				action = new ChangeColorMode();
			}

			if ( action != nullptr )
			{
				try
				{
					action->setParameters( words );
				}
				catch ( const std::exception & )
				{
					action->displayUsage();
					delete action;
					return;
				}

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

		void ActionManager::update( const double p_deltaTime )
		{
			while ( _actionQueue.empty() == false )
			{
				_flushAction( _actionQueue.front() );
				_actionQueue.pop();
			}
		}

		void ActionManager::_flushAction( BaseAction * p_action )
		{
			p_action->execute();

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
			while ( _bufferUndo.size() > ACTION_BUFFER_SIZE )
			{
				delete _bufferUndo.back();
				_bufferUndo.pop_back();
			}
		}
	} // namespace Action
} // namespace VTX
