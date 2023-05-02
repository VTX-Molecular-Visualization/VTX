#ifndef __VTX_APP_RENDER_RENDERER_RT_BASE_MATERIAL__
#define __VTX_APP_RENDER_RENDERER_RT_BASE_MATERIAL__

#include "../intersection.hpp"
#include "../lights/light_sample.hpp"
#include "../ray.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Render::Renderer::RT::Material
{
	class BaseMaterial
	{
	  public:
		BaseMaterial()			= default;
		virtual ~BaseMaterial() = default;

		virtual Util::Color::Rgba getColor() const = 0;

		virtual Util::Color::Rgba shade( const Ray &				p_ray,
										 const Intersection &		p_hit,
										 const Light::LightSample & p_lightDirSample ) const
			= 0;
	};
} // namespace VTX::App::Render::Renderer::RT::Material

#endif
