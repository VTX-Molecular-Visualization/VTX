#ifndef __VTX_ACTION_CHANGE_THEME__
#define __VTX_ACTION_CHANGE_THEME__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeTheme : public BaseAction
		{
		  public:
			virtual void execute() override { VTXApp::get().getUI().setTheme(); };
		};
	} // namespace Action
} // namespace VTX
#endif
