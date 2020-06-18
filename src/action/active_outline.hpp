#ifndef __VTX_ACTION_ACTIVE_OUTLINE__
#define __VTX_ACTION_ACTIVE_OUTLINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveOutline : public BaseAction
		{
		  public:
			explicit ActiveOutline( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override
			{
				VTX_SETTING().activeOutline = _active;
				VTXApp::get().getRendererGL().activeOutline( _active );
			};

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
