#include "action_manager.hpp"
#include "action_snapshot.hpp"

namespace VTX
{
	namespace Action
	{
		void ActionManager::executeAction( BaseAction * const p_action )
		{
			// Execute.
			p_action->execute();

			// Handle undo.
			BaseActionUndonable * undonable = dynamic_cast<BaseActionUndonable *>( p_action );
			if ( undonable != nullptr )
			{
				_bufferUndo.push_front( undonable );
				_purgeBuffer( undonable );
			}
			else
			{
				delete p_action;
			}

			// Clear redo actions.
			for ( BaseActionUndonable * action : _bufferRedo )
			{
				delete action;
			}
			_bufferRedo.clear();
		}

		void ActionManager::executeAction( const std::string & p_action )
		{
			// TODO: extract args from string.
			// TODO: map string to class with variadics (not possible in cpp, no reflection).
			// Name action with enum?
			if ( p_action == "snapshot" ) { executeAction( new ActionSnapshot() ); }
		}

		bool ActionManager::canUndo() const { return _bufferUndo.size() > 0; }

		void ActionManager::undo()
		{
			if ( canUndo() == false ) { return; }

			VTX_DEBUG( "Undo" );
			_bufferUndo.front()->undo();
			_bufferRedo.push_front( _bufferUndo.front() );
			_bufferUndo.pop_front();
		}

		bool ActionManager::canRedo() const { return _bufferRedo.size() > 0; }

		void ActionManager::redo()
		{
			if ( canRedo() == false ) { return; }

			VTX_DEBUG( "Redo" );
			_bufferRedo.front()->redo();
			_bufferUndo.push_front( _bufferRedo.front() );
			_bufferRedo.pop_front();
		}

		void ActionManager::_purgeBuffer( BaseActionUndonable * const p_action )
		{
			while ( _bufferUndo.size() > ACTION_BUFFER_SIZE )
			{
				delete _bufferUndo.back();
				_bufferUndo.pop_back();
			}
		}
	} // namespace Action
} // namespace VTX
