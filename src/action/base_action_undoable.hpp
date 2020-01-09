#ifndef __VTX_BASE_ACTION_UNDOABLE__
#define __VTX_BASE_ACTION_UNDOABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class BaseActionUndoable : public BaseAction
		{
		  public:
			virtual void undo() = 0;
		};
	} // namespace Action
} // namespace VTX
#endif
