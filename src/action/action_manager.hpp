#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

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
			void executeAction( BaseAction * const p_action ) const
			{
				p_action->execute();
				delete p_action;
			}
			void executeAction( const std::string & p_action ) const
			{
				// TODO: extract args from string.
				// TODO: map string to class with variadics (not possible in cpp,, no reflection).
				if ( p_action == "snapshot" ) { executeAction( new ActionSnapshot() ); }
			}

			// bool canUndo()
			// void undo()
			// bool canRedo()
			// void redo()

		  private:
			std::list<BaseAction *> _buffer = std::list<BaseAction *>();
		};
	} // namespace Action
} // namespace VTX
#endif
