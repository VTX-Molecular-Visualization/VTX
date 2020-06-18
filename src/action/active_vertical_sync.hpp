#ifndef __VTX_ACTION_ACTIVE_VERTICAL_SYNC__
#define __VTX_ACTION_ACTIVE_VERTICAL_SYNC__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveVerticalSync : public BaseAction
		{
		  public:
			explicit ActiveVerticalSync( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override
			{
				VTX_SETTING().activeVSync = _active;
				VTXApp::get().getUI().setVSync( VTX_SETTING().activeVSync );
			};

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
