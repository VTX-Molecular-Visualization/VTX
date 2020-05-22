#ifndef __VTX_ACTION_CHANGE_AO_FACTOR__
#define __VTX_ACTION_CHANGE_AO_FACTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAOFactor : public BaseAction
		{
		  public:
			explicit ChangeAOFactor( const float p_factor ) : _factor( Util::Math::clamp( p_factor, 0.f, 1.f ) ) {}

			virtual void execute() override { Setting::Rendering::aoFactor = _factor; };

		  private:
			const float _factor;
		};
	} // namespace Action
} // namespace VTX
#endif
