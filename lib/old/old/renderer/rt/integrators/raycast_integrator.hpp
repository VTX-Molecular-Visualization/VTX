#ifndef __VTX_RENDERER_RAY_TRACER_RAYCAST_INTEGRATOR__
#define __VTX_RENDERER_RAY_TRACER_RAYCAST_INTEGRATOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_integrator.hpp"

namespace VTX
{
	namespace Renderer
	{
		// TODO: rename
		class RayCastIntegrator : public BaseIntegrator
		{
		  public:
			Color::Rgba Li( const Ray &	  p_ray,
							const Scene & p_scene,
							const float	  p_tMin,
							const float	  p_tMax ) const override;

		  private:
		};

	} // namespace Renderer
} // namespace VTX

#endif
