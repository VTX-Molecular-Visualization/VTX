#ifndef __VTX_RENDERER_RAY_TRACER_BASE_INTEGRATOR__
#define __VTX_RENDERER_RAY_TRACER_BASE_INTEGRATOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "../ray.hpp"
#include "../rtScene.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseIntegrator
		{
		  public:
			virtual ~BaseIntegrator() = default;

			virtual Vec3f Li( const Ray &	p_ray,
							  const Scene & p_scene,
							  const float	p_tMin,
							  const float	p_tMax ) const = 0;

		  protected:
			// TODO: static
			const Vec3f _backgroundColor = Vec3f( 0.f, 0.7f, 1.f );
		};

	} // namespace Renderer
} // namespace VTX

#endif
