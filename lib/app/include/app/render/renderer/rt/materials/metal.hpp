#ifndef __VTX_APP_RENDER_RENDERER_RT_MATERIAL_METAL__
#define __VTX_APP_RENDER_RENDERER_RT_MATERIAL_METAL__

#include "../brdfs/cook_torrance.hpp"
#include "base_material.hpp"
#include <util/math.hpp>

namespace VTX::App::Render::Renderer::RT::Material
{
	class MetalMaterial : public BaseMaterial
	{
	  public:
		MetalMaterial( const Util::Color::Rgba & p_f0, const float p_shininess ) :
			_f0( p_f0 ), _shininess( p_shininess )
		{
		}

		Util::Color::Rgba getColor() const override { return _f0; }

		Util::Color::Rgba shade( const Ray &				p_ray,
								 const Intersection &		p_hit,
								 const Light::LightSample & p_lightSample ) const override
		{
			// metals don't produce diffuse reflections.
			return BRDF::CookTorrance::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _f0, _shininess );
		}

		static MetalMaterial * createAluminium()
		{
			return new MetalMaterial( Util::Color::Rgba( 0.913f, 0.922f, 0.924f ), 128.f );
		}
		static MetalMaterial * createCopper()
		{
			return new MetalMaterial( Util::Color::Rgba( 0.955f, 0.638f, 0.538f ), 32.f );
		}
		static MetalMaterial * createGold()
		{
			return new MetalMaterial( Util::Color::Rgba( 1.022f, 0.782f, 0.344f ), 64.f );
		}
		static MetalMaterial * createZinc()
		{
			return new MetalMaterial( Util::Color::Rgba( 0.644f, 0.824f, 0.850f ), 64.f );
		}

	  private:
		Util::Color::Rgba _f0		 = Util::Color::Rgba::WHITE;
		float			  _shininess = 64.f;
	};
} // namespace VTX::App::Render::Renderer::RT::Material

#endif
