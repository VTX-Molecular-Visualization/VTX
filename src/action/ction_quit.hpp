#ifndef __VTX_ACTION_QUIT__
#define __VTX_ACTION_QUIT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Quit : public BaseAction
		{
		  public:
			virtual void execute() override { VTXApp::get().stop(); };
		};
	} // namespace Action
} // namespace VTX
#endif
