#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "action_snapshot.hpp"
#include "base_action.hpp"
#include <list>
#include <string>

namespace VTX
{
	namespace Action
	{
		class ActionManager
		{
		  public:
			void executeAction( BaseAction * const p_action )
			{
				p_action->execute();

				if ( p_action->canBeUndone() )
				{
					_handleBuffer( p_action );
					return;
				}

				delete p_action;
			}

			void executeAction( const std::string & p_action )
			{
				// TODO: extract args from string.
				// TODO: map string to class with variadics (not possible in cpp,, no reflection).
				if ( p_action == "snapshot" ) { executeAction( new ActionSnapshot() ); }
			}

			bool canUndo() const { return _buffer.size() > 0; }

			void undo()
			{
				if ( canUndo() == false ) { return; }

				VTX_DEBUG( "Undo" );
				_buffer.front()->undo();
				delete _buffer.front();
				_buffer.pop_front();
			}

		  private:
			std::list<BaseAction *> _buffer = std::list<BaseAction *>();

			void _handleBuffer( BaseAction * const p_action )
			{
				// Push and remove ancients.
				_buffer.push_front( p_action );
				while ( _buffer.size() > ACTION_BUFFER_SIZE )
				{
					delete _buffer.back();
					_buffer.pop_back();
				}
			}
		};
	} // namespace Action
} // namespace VTX
#endif
