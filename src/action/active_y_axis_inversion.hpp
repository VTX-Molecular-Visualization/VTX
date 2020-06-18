#ifndef __VTX_ACTION_ACTIVE_Y_AXIS_INVERSION__
#define __VTX_ACTION_ACTIVE_Y_AXIS_INVERSION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveYAxisInversion : public BaseAction
		{
		  public:
			explicit ActiveYAxisInversion( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override { VTX_SETTING().yAxisInverted = _active; };

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
