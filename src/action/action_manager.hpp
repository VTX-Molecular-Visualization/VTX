#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionManager
		{
		  public:
			void runAction( BaseAction & p_action ) const { p_action.run(); }
		};
	} // namespace Action
} // namespace VTX
#endif
