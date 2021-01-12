#ifndef __VTX_BASE_ACTION_UNDONABLE__
#define __VTX_BASE_ACTION_UNDONABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class BaseActionUndonable : public BaseAction
		{
		  public:
			virtual ~BaseActionUndonable() = default;

			virtual void undo() = 0;
			virtual void redo() { execute(); }
		};
	} // namespace Action
} // namespace VTX
#endif
