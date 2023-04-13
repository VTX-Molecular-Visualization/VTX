#include "action_manager.hpp"
#include "tool/logger.hpp"
#include <sstream>
#include <string>

// TODO VTX_LOGIC Manage scene modification

namespace VTX
{
	namespace Action
	{
		void ActionManager::execute( BaseAction * const p_action ) { _flushAction( p_action ); }

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
			case ACTION_TAG::MODIFY_SCENE:
				// VTXApp::get().getScenePathData().decrementSceneModifications();
				break;
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
			case ACTION_TAG::MODIFY_SCENE:
				// VTXApp::get().getScenePathData().incrementSceneModifications();
				break;
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

		void ActionManager::setActionBufferSize( const int p_size )
		{
			_actionBufferSize = p_size;
			_purgeBuffer();
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
					// if ( isActionUndonable )
					// VTXApp::get().getScenePathData().incrementSceneModifications();
					// else // if the action is not undoable, it make a permanent modification on scene
					// VTXApp::get().getScenePathData().forceSceneModifications();
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
			while ( _bufferUndo.size() > _actionBufferSize )
			{
				delete _bufferUndo.back();
				_bufferUndo.pop_back();
			}

			// If we loose the last action which modify scene before saving, the scene can't return to unmodified state
			// if ( VTXApp::get().getScenePathData().getSceneModificationsCounter() < 0 )
			//{
			//	VTXApp::get().getScenePathData().forceSceneModifications();
			//}
		}
	} // namespace Action
} // namespace VTX
