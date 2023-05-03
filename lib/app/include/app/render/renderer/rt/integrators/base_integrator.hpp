#ifndef __VTX_APP_RENDER_RENDERER_RT_BASE_INTEGRATOR__
#define __VTX_APP_RENDER_RENDERER_RT_BASE_INTEGRATOR__

#include "../ray.hpp"
#include "../rt_scene.hpp"
#include <util/types.hpp>

namespace VTX::App::Render::Renderer::RT::Integrator
{
	class BaseIntegrator
	{
	  public:
		BaseIntegrator()		  = default;
		virtual ~BaseIntegrator() = default;

		virtual Util::Color::Rgba Li( const Ray &	p_ray,
									  const Scene & p_scene,
									  const float	p_tMin,
									  const float	p_tMax ) const
			= 0;

	  protected:
		static constexpr float SHADOW_EPS = 0.00001f;

		// TODO: static
		const Util::Color::Rgba _backgroundColor //
												 //	= Vec3f( 1.5f ) * 0.1f;
			= Util::Color::Rgba( 0.5f, 0.6f, 0.8f ) * 0.05f;
		//( 0.7f, 0.7f, 0.7f );
	};

} // namespace VTX::App::Render::Renderer::RT::Integrator

#endif
