#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_DIRECT_LIGHTING_INTEGRATOR__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_DIRECT_LIGHTING_INTEGRATOR__

#include "base_integrator.hpp"

namespace VTX::App::Old::Render::Renderer::RT::Integrator
{
	class DirectLightingIntegrator : public BaseIntegrator
	{
	  public:
		Util::Color::Rgba Li( const Ray &	p_ray,
							  const Scene & p_scene,
							  const float	p_tMin,
							  const float	p_tMax ) const override;

	  private:
	};

} // namespace VTX::App::Old::Render::Renderer::RT::Integrator

#endif