#ifndef __VTX_ACTION_NEW__
#define __VTX_ACTION_NEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class New : public BaseAction
		{
		  public:
			virtual void execute() override { VTXApp::get().getScene().clear(); };
		};
	} // namespace Action
} // namespace VTX
#endif
