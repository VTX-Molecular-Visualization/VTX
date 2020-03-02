#include "action_manager.hpp"
#include "snapshot.hpp"
#include <thread>

namespace VTX
{
	namespace Action
	{
		void ActionManager::action( BaseAction * const p_action )
		{
			p_action->execute();

			// Handle undo.
			BaseActionUndonable * undonable = dynamic_cast<BaseActionUndonable *>( p_action );
			if ( undonable != nullptr )
			{
				_bufferUndo.push_front( undonable );
				_purgeBuffer();
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

		void ActionManager::action( const std::string & p_action )
		{
			// TODO: extract args from string.
			// TODO: map string to class with variadics (not possible in cpp, no reflection).
			// Name action with enum?
			if ( p_action == "snapshot" ) { action( new Snapshot() ); }
		}

		bool ActionManager::canUndo() const { return _bufferUndo.size() > 0; }

		void ActionManager::undo()
		{
			if ( canUndo() == false ) { return; }

			VTX_DEBUG( "Undo (" + std::to_string( _bufferUndo.size() - 1 ) + " more)" );
			_bufferUndo.front()->undo();
			_bufferRedo.push_front( _bufferUndo.front() );
			_bufferUndo.pop_front();
		}

		bool ActionManager::canRedo() const { return _bufferRedo.size() > 0; }

		void ActionManager::redo()
		{
			if ( canRedo() == false ) { return; }

			VTX_DEBUG( "Redo (" + std::to_string( _bufferRedo.size() - 1 ) + " more)" );
			_bufferRedo.front()->redo();
			_bufferUndo.push_front( _bufferRedo.front() );
			_bufferRedo.pop_front();
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
