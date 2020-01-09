#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
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
			void executeAction( std::string & p_action ) const
			{
				// TODO: extract args from string.
				// TODO: map string to class with variadics (if possible).
			}

			// bool canUndo()
			// void undo()
			// bool canRedo()
			// void redo()
			// store the stack of the x last actions
		};
	} // namespace Action
} // namespace VTX
#endif
