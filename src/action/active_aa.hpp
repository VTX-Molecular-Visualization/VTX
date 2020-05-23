#ifndef __VTX_ACTION_ACTIVE_AA__
#define __VTX_ACTION_ACTIVE_AA__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveAA : public BaseAction
		{
		  public:
			explicit ActiveAA( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override
			{
				Setting::Rendering::useAA = _active;
				VTXApp::get().getRendererGL().activeAA( _active );
			};

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
