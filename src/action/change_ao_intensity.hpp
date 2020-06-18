#ifndef __VTX_ACTION_CHANGE_AO_INTENSITY__
#define __VTX_ACTION_CHANGE_AO_INTENSITY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAOIntensity : public BaseAction
		{
		  public:
			explicit ChangeAOIntensity( const int p_intensity ) : _intensity( p_intensity ) {}

			virtual void execute() override
			{
				VTX_SETTING().aoIntensity
					= Util::Math::clamp( _intensity, Setting::AO_INTENSITY_MIN, Setting::AO_INTENSITY_MAX );
			};

		  private:
			const int _intensity;
		};
	} // namespace Action
} // namespace VTX
#endif
