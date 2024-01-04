#include "app/application/action/action_manager.hpp"

namespace VTX::App::Application::Action
{
	ActionManager::ActionManager()	= default;
	ActionManager::~ActionManager() = default;
	void ActionManager::execute( std::unique_ptr<BaseAction> & p_actionPtr )
	{
		try
		{
			p_actionPtr->execute();
			// VTX_EVENT( ACTION_EXECUTED_EVENT, *actionPtr );
			// // Signal catched and managed by ScenePathData component
			// switch ( p_action->getTag() )
			//{
			// case ACTION_TAG::MODIFY_SCENE:
			//	if ( isActionUndonable )
			//		App::Old::VTXApp::get().getScenePathData().incrementSceneModifications();
			//	else // if the action is not undoable, it make a permanent modification on scene
			//		App::Old::VTXApp::get().getScenePathData().forceSceneModifications();
			//	break;
			// case ACTION_TAG::NONE:
			// default: break;
			//}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error with action of type {} : {}", typeid( *p_actionPtr ).name(), p_e.what() );
			return;
		}
	}
	void ActionManager::execute( std::unique_ptr<BaseActionUndonable> & p_actionPtr )
	{
		try
		{
			p_actionPtr->execute();
			// VTX_EVENT( ACTION_EXECUTED_EVENT, *actionPtr );
			// // Signal catched and managed by ScenePathData component
			// switch ( p_action->getTag() )
			//{
			// case ACTION_TAG::MODIFY_SCENE:
			//	if ( isActionUndonable )
			//		App::Old::VTXApp::get().getScenePathData().incrementSceneModifications();
			//	else // if the action is not undoable, it make a permanent modification on scene
			//		App::Old::VTXApp::get().getScenePathData().forceSceneModifications();
			//	break;
			// case ACTION_TAG::NONE:
			// default: break;
			//}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Error with action of type {} : {}", typeid( *p_actionPtr ).name(), p_e.what() );
			return;
		}

		// Handle undo.
		_bufferUndo.push_front( std::move( p_actionPtr ) );
		_purgeBuffer();

		_bufferRedo.clear();
	}

	bool ActionManager::canUndo() const { return _bufferUndo.size() > 0; }

	void ActionManager::undo()
	{
		if ( !canUndo() )
			return;

		VTX_DEBUG( "Undo ( {} more )", ( _bufferUndo.size() - 1 ) );
		std::unique_ptr<BaseActionUndonable> & actionToUndo = _bufferUndo.front();

		actionToUndo->undo();
		_bufferRedo.push_front( std::move( actionToUndo ) );
		_bufferUndo.pop_front();

		// VTX_EVENT( "UNDO_ACTION", *actionToUndo );
		// switch ( actionToUndo->getTag() )
		//{
		// case ACTION_TAG::MODIFY_SCENE: App::Old::VTXApp::get().getScenePathData().decrementSceneModifications();
		// break; case ACTION_TAG::NONE: default: break;
		// }
	}

	bool ActionManager::canRedo() const { return _bufferRedo.size() > 0; }

	void ActionManager::redo()
	{
		if ( !canRedo() )
			return;

		VTX_DEBUG( "Redo ( {} more ).", ( _bufferRedo.size() - 1 ) );

		std::unique_ptr<BaseActionUndonable> & actionToRedo = _bufferRedo.front();
		actionToRedo->redo();
		_bufferUndo.push_front( std::move( actionToRedo ) );
		_bufferRedo.pop_front();

		// VTX_EVENT( "REDO_ACTION", *actionToUndo );
		// switch ( actionToRedo->getTag() )
		//{
		// case ACTION_TAG::MODIFY_SCENE: App::Old::VTXApp::get().getScenePathData().incrementSceneModifications();
		// break; case ACTION_TAG::NONE: default: break;
		//}
	}

	void ActionManager::setBufferSize( const uint p_bufferSize )
	{
		_bufferSize = p_bufferSize;
		_purgeBuffer();
	}

	void ActionManager::_purgeBuffer()
	{
		if ( _bufferSize == 0 )
			return;

		while ( _bufferUndo.size() > _bufferSize )
		{
			// const std::unique_ptr<BaseActionUndonable> & purgedAction = _bufferUndo.back();
			//  VTX_EVENT( "PURGE_ACTION", *purgedAction );
			//  If we loose the last action which modify scene before saving, the scene can't return to unmodified
			//  state if ( App::Old::VTXApp::get().getScenePathData().getSceneModificationsCounter() < 0 )
			//{
			//	App::Old::VTXApp::get().getScenePathData().forceSceneModifications();
			// }

			_bufferUndo.pop_back();
		}
	}
} // namespace VTX::App::Application::Action
