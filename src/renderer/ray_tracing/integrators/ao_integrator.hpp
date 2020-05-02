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
			AOIntegrator() = default;
			AOIntegrator( const float p_radius, const float p_intensity, const uint p_nbSamples ) :
				_radius( Util::Math::max( 0.f, p_radius ) ), _intensity( Util::Math::max( 1.f, p_intensity ) ),
				_nbSamples( Util::Math::max( 1u, p_nbSamples ) )
			{
			}

			Vec3f Li( const Ray & p_ray, const Scene & p_scene, const float p_tMin, const float p_tMax ) const override;

		  private:
			float _radius	 = 50.f;
			float _intensity = 1.f;
			uint  _nbSamples = 1024;
		};

	} // namespace Renderer
} // namespace VTX

#endif
