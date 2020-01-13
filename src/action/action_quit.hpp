#ifndef __VTX_ACTION_QUIT__
#define __VTX_ACTION_QUIT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionQuit : public BaseAction
		{
		  public:
			virtual void execute() override { VTXApp::get().stop(); };
		};
	} // namespace Action
} // namespace VTX
#endif
