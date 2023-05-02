#ifndef __VTX_APP_RENDER_RENDERER_RT_RAYCAST_INTEGRATOR__
#define __VTX_APP_RENDER_RENDERER_RT_RAYCAST_INTEGRATOR__

#include "base_integrator.hpp"

namespace VTX::App::Render::Renderer::RT::Integrator
{
	// TODO: rename
	class RayCastIntegrator : public BaseIntegrator
	{
	  public:
		Util::Color::Rgba Li( const Ray &	p_ray,
							  const Scene & p_scene,
							  const float	p_tMin,
							  const float	p_tMax ) const override;

	  private:
	};

} // namespace VTX::App::Render::Renderer::RT::Integrator

#endif
