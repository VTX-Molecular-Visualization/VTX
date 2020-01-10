#include "action_manager.hpp"
#include "action_snapshot.hpp"

namespace VTX
{
	namespace Action
	{
		void ActionManager::executeAction( BaseAction * const p_action )
		{
			p_action->execute();

			BaseActionUndonable * undonable = dynamic_cast<BaseActionUndonable *>( p_action );
			if ( undonable != nullptr ) { _handleBuffer( undonable ); }
			else
			{
				delete p_action;
			}
		}

		void ActionManager::executeAction( const std::string & p_action )
		{
			// TODO: extract args from string.
			// TODO: map string to class with variadics (not possible in cpp,, no reflection).
			if ( p_action == "snapshot" ) { executeAction( new ActionSnapshot() ); }
		}

		bool ActionManager::canUndo() const { return _buffer.size() > 0; }

		void ActionManager::undo()
		{
			if ( canUndo() == false ) { return; }

			VTX_DEBUG( "Undo" );
			_buffer.front()->undo();
			delete _buffer.front();
			_buffer.pop_front();
		}

		void ActionManager::_handleBuffer( BaseActionUndonable * const p_action )
		{
			// Push and remove ancients.
			_buffer.push_front( p_action );
			while ( _buffer.size() > ACTION_BUFFER_SIZE )
			{
				delete _buffer.back();
				_buffer.pop_back();
			}
		}
	} // namespace Action
} // namespace VTX
