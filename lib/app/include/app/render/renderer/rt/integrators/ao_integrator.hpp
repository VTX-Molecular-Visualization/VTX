#ifndef __VTX_APP_RENDER_RENDERER_RT_AO_INTEGRATOR__
#define __VTX_APP_RENDER_RENDERER_RT_AO_INTEGRATOR__

#include "base_integrator.hpp"

namespace VTX::App::Render::Renderer::RT::Integrator
{
	class AOIntegrator : public BaseIntegrator
	{
	  public:
		AOIntegrator() = default;
		AOIntegrator( const float p_radius, const uint p_nbSamples ) :
			_radius( Util::Math::max( 0.f, p_radius ) ), _nbSamples( Util::Math::max( 1u, p_nbSamples ) )
		{
		}

		Util::Color::Rgba Li( const Ray &	p_ray,
							  const Scene & p_scene,
							  const float	p_tMin,
							  const float	p_tMax ) const override;

	  private:
		float _radius	 = 20.f;
		uint  _nbSamples = 32;
	};

} // namespace VTX::App::Render::Renderer::RT::Integrator

#endif
