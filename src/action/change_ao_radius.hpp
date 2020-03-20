#ifndef __VTX_ACTION_CHANGE_AO_RADIUS__
#define __VTX_ACTION_CHANGE_AO_RADIUS__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeAORadius : public BaseAction
		{
		  public:
			explicit ChangeAORadius( const float p_radius ) : _radius( p_radius ) {}

			virtual void execute() override
			{
				Setting::Rendering::aoRadius
					= Util::Math::clamp( _radius, RENDERER_AO_RADIUS_MIN, RENDERER_AO_RADIUS_MAX );
			};

		  private:
			const float _radius;
		};
	} // namespace Action
} // namespace VTX
#endif
