#include "app/core/action/action_manager.hpp"
#include "app/action/main.hpp"
#include "app/action/representable.hpp"
#include "app/action/setting.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/io/struct/scene_path_data.hpp"
#include "app/old_app/model/chain.hpp"
#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/representation/representation_library.hpp"
#include "app/old_app/model/residue.hpp"
#include "app/old_app/vtx_app.hpp"
#include <magic_enum.hpp>
#include <sstream>

namespace VTX::Core::Action
{
	void ActionManager::execute( BaseAction * const p_action ) { _flushAction( p_action ); }

	// TODO interpret commands outside of this class. The CommandInterpretor class will generate an action and call
	// ActionManager::execute(BaseAction * const p_action)
	void ActionManager::execute( const std::string & p_action )
	{
		// std::istringstream		 iss( p_action );
		// std::string				 word;
		// std::vector<std::string> words = std::vector<std::string>();
		// while ( iss >> word )
		//{
		//	words.emplace_back( word );
		// }

		// if ( words.size() == 0 )
		//{
		//	VTX_ERROR( "Empty action string" );
		// }

		// std::string & command = words[ 0 ];
		// BaseAction *  action  = nullptr;

		//// TODO: map with ids.
		// try
		//{
		//	Model::Molecule & molecule = *( *VTXApp::get().getScene().getMolecules().begin() ).first;

		//	if ( command == "snapshot" )
		//	{
		//		action = new VTX::Action::Main::Snapshot(
		//			Worker::Snapshoter::MODE::GL,
		//			IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ) );
		//	}
		//	else if ( command == "change_representation" )
		//	{
		//		action = new VTX::Action::Setting::ChangeDefaultRepresentation( std::stoi( words.at( 1 ) ) );
		//	}
		//	else if ( command == "change_shading" )
		//	{
		//		action = new VTX::Action::Setting::ChangeShading(
		//			magic_enum::enum_cast<Renderer::SHADING>( words.at( 1 ) ).value() );
		//	}
		//	else if ( command == "change_color_mode" )
		//	{
		//		action = new VTX::Action::Setting::ChangeColorMode(
		//			magic_enum::enum_cast<Generic::COLOR_MODE>( words.at( 1 ) ).value() );
		//	}
		//	else if ( command == "set_representation_molecule" )
		//	{
		//		Model::Representation::Representation * const representation
		//			= Model::Representation::RepresentationLibrary::get().getRepresentationByName( words.at( 1 ) );
		//		action = new VTX::Action::Representable::SetRepresentation( molecule, representation );
		//	}
		//	else if ( command == "remove_representation_molecule" )
		//	{
		//		action = new VTX::Action::Representable::RemoveRepresentation( molecule );
		//	}
		//	else if ( command == "set_representation_chain" )
		//	{
		//		Model::Representation::Representation * const representation
		//			= Model::Representation::RepresentationLibrary::get().getRepresentationByName( words.at( 1 ) );
		//		const int idChain = std::stoi( words.at( 2 ) );
		//		action			  = new VTX::Action::Representable::SetRepresentation( *molecule.getChains()[ idChain ],
		//																	   representation );
		//	}
		//	else if ( command == "remove_representation_chain" )
		//	{
		//		const int idChain = std::stoi( words.at( 1 ) );
		//		action = new VTX::Action::Representable::RemoveRepresentation( *molecule.getChains()[ idChain ] );
		//	}
		//	else if ( command == "set_representation_residue" )
		//	{
		//		Model::Representation::Representation * const representation
		//			= Model::Representation::RepresentationLibrary::get().getRepresentationByName( words.at( 1 ) );
		//		const int indexResidue = std::stoi( words.at( 2 ) );
		//		action = new VTX::Action::Representable::SetRepresentation( *molecule.getResidues()[ indexResidue ],
		//																	representation );
		//	}
		//	else if ( command == "remove_representation_residue" )
		//	{
		//		const int indexResidue = std::stoi( words.at( 1 ) );
		//		action
		//			= new VTX::Action::Representable::RemoveRepresentation( *molecule.getResidues()[ indexResidue ] );
		//	}
		//}
		// catch ( const std::exception & )
		//{
		//	if ( action != nullptr )
		//	{
		//		action->displayUsage();
		//		delete action;
		//		return;
		//	}
		//}

		// if ( action != nullptr )
		//{
		//	execute( action );
		// }
		// else
		//{
		//	VTX_ERROR( "Action not found" );
		// }
	}

	bool ActionManager::canUndo() const { return _bufferUndo.size() > 0; }

	void ActionManager::undo()
	{
		if ( canUndo() == false )
		{
			return;
		}

		VTX_DEBUG( "Undo (" + std::to_string( _bufferUndo.size() - 1 ) + " more)" );
		BaseActionUndonable * const actionToUndo = _bufferUndo.front();

		actionToUndo->undo();
		_bufferRedo.push_front( actionToUndo );
		_bufferUndo.pop_front();

		switch ( actionToUndo->getTag() )
		{
		case ACTION_TAG::MODIFY_SCENE: VTXApp::get().getScenePathData().decrementSceneModifications(); break;
		case ACTION_TAG::NONE:
		default: break;
		}
	}

	bool ActionManager::canRedo() const { return _bufferRedo.size() > 0; }

	void ActionManager::redo()
	{
		if ( canRedo() == false )
		{
			return;
		}

		VTX_DEBUG( "Redo (" + std::to_string( _bufferRedo.size() - 1 ) + " more)" );
		BaseActionUndonable * const actionToRedo = _bufferRedo.front();
		actionToRedo->redo();
		_bufferUndo.push_front( actionToRedo );
		_bufferRedo.pop_front();

		switch ( actionToRedo->getTag() )
		{
		case ACTION_TAG::MODIFY_SCENE: VTXApp::get().getScenePathData().incrementSceneModifications(); break;
		case ACTION_TAG::NONE:
		default: break;
		}
	}

	void ActionManager::update( const float & p_deltaTime )
	{
		while ( _actionQueue.empty() == false )
		{
			_flushAction( _actionQueue.front() );
			_actionQueue.pop();
		}
	}

	void ActionManager::_flushAction( BaseAction * const p_action )
	{
		BaseActionUndonable * undonable			= dynamic_cast<BaseActionUndonable *>( p_action );
		const bool			  isActionUndonable = undonable != nullptr;

		try
		{
			p_action->execute();
			switch ( p_action->getTag() )
			{
			case ACTION_TAG::MODIFY_SCENE:
				if ( isActionUndonable )
					VTXApp::get().getScenePathData().incrementSceneModifications();
				else // if the action is not undoable, it make a permanent modification on scene
					VTXApp::get().getScenePathData().forceSceneModifications();
				break;
			case ACTION_TAG::NONE:
			default: break;
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
		}

		// Handle undo.
		/*
		if ( isActionUndonable )
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
		{*/
		delete p_action;
		//}
	}

	void ActionManager::_purgeBuffer()
	{
		while ( _bufferUndo.size() > VTX::Setting::ACTION_BUFFER_SIZE )
		{
			delete _bufferUndo.back();
			_bufferUndo.pop_back();
		}

		// If we loose the last action which modify scene before saving, the scene can't return to unmodified state
		if ( VTXApp::get().getScenePathData().getSceneModificationsCounter() < 0 )
		{
			VTXApp::get().getScenePathData().forceSceneModifications();
		}
	}
} // namespace VTX::Core::Action
