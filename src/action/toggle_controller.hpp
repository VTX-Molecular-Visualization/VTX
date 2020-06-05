#ifndef __VTX_ACTION_TOGGLE_CONTROLLER__
#define __VTX_ACTION_TOGGLE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ToggleController : public BaseAction
		{
		  public:
			explicit ToggleController() {}

			virtual void execute() override { };
		};
	} // namespace Action
} // namespace VTX
#endif
