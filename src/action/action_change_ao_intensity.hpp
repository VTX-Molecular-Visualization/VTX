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
		class ActionChangeAOIntensity : public BaseAction
		{
		  public:
			explicit ActionChangeAOIntensity( const int p_intensity ) : _intensity( p_intensity ) {}

			virtual void execute() override { Setting::Rendering::aoIntensity = _intensity; };

		  private:
			const int _intensity;
		};
	} // namespace Action
} // namespace VTX
#endif
