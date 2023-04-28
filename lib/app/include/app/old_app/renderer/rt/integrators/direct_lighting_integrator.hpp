#ifndef __VTX_RENDERER_RAY_TRACER_DIRECT_LIGHTING_INTEGRATOR__
#define __VTX_RENDERER_RAY_TRACER_DIRECT_LIGHTING_INTEGRATOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_integrator.hpp"

namespace VTX
{
	namespace Renderer
	{
		class DirectLightingIntegrator : public BaseIntegrator
		{
		  public:
			Util::Color::Rgba Li( const Ray &	  p_ray,
							const Scene & p_scene,
							const float	  p_tMin,
							const float	  p_tMax ) const override;

		  private:
		};

	} // namespace Renderer
} // namespace VTX

#endif
