#ifndef __VTX_ACTION_ACTIVE_SSAO__
#define __VTX_ACTION_ACTIVE_SSAO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveSSAO : public BaseAction
		{
		  public:
			explicit ActiveSSAO( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override
			{
				VTX_SETTING().activeAO = _active;
				VTXApp::get().getRendererGL().activeSSAO( _active );
			};

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
