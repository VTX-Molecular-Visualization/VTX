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
		class ActionActiveSSAO : public BaseAction
		{
		  public:
			explicit ActionActiveSSAO( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override { Setting::Rendering::useSSAO = _active; };

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
