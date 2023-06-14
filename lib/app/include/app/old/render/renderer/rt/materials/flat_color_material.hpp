#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_FLAT_COLOR_MATERIAL__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_FLAT_COLOR_MATERIAL__

#include "base_material.hpp"

namespace VTX::App::Old::Render::Renderer::RT::Material
{
	class FlatColorMaterial : public BaseMaterial
	{
	  public:
		FlatColorMaterial( const Util::Color::Rgba & p_color ) : _color( p_color ) {}

		Util::Color::Rgba getColor() const override { return _color; }

		Util::Color::Rgba shade( const Ray &				p_ray,
								 const Intersection &		p_hit,
								 const Light::LightSample & p_lightSample ) const override
		{
			return _color;
		}

	  private:
		Util::Color::Rgba _color;
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Material

#endif
