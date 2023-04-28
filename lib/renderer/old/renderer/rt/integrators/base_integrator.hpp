#ifndef __VTX_RENDERER_RAY_TRACER_BASE_INTEGRATOR__
#define __VTX_RENDERER_RAY_TRACER_BASE_INTEGRATOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "../ray.hpp"
#include "../rt_scene.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseIntegrator
		{
		  public:
			BaseIntegrator()		  = default;
			virtual ~BaseIntegrator() = default;

			virtual Util::Color::Rgba Li( const Ray &	  p_ray,
									const Scene & p_scene,
									const float	  p_tMin,
									const float	  p_tMax ) const
				= 0;

		  protected:
			static constexpr float SHADOW_EPS = 0.00001f;

			// TODO: static
			const Util::Color::Rgba _backgroundColor //
											   //	= Vec3f( 1.5f ) * 0.1f;
				= Util::Color::Rgba( 0.5f, 0.6f, 0.8f ) * 0.05f;
			//( 0.7f, 0.7f, 0.7f );
		};

	} // namespace Renderer
} // namespace VTX

#endif
