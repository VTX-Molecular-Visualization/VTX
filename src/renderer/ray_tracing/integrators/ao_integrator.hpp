#ifndef __VTX_RENDERER_RAY_TRACER_AO_INTEGRATOR__
#define __VTX_RENDERER_RAY_TRACER_AO_INTEGRATOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_integrator.hpp"

namespace VTX
{
	namespace Renderer
	{
		class AOIntegrator : public BaseIntegrator
		{
		  public:
			Vec3f Li( const Ray & p_ray, const Scene & p_scene, const float p_tMin, const float p_tMax ) const override;

		  private:
		};

	} // namespace Renderer
} // namespace VTX

#endif
