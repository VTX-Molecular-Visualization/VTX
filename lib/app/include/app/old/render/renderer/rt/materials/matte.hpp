#ifndef __VTX_APP_OLD_RENDER_RENDERER_RT_MATERIAL_MATTE__
#define __VTX_APP_OLD_RENDER_RENDERER_RT_MATERIAL_MATTE__

#include "../brdfs/lambert.hpp"
#include "../brdfs/oren_nayar.hpp"
#include "base_material.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Render::Renderer::RT::Material
{
	class MatteMaterial : public BaseMaterial
	{
	  public:
		MatteMaterial( const Util::Color::Rgba & p_kd, const float p_roughness = 0.f ) :
			_kd( p_kd ), _roughness( p_roughness )
		{
		}

		Util::Color::Rgba getColor() const override { return _kd; }

		Util::Color::Rgba shade( const Ray &				p_ray,
								 const Intersection &		p_hit,
								 const Light::LightSample & p_lightSample ) const override
		{
			// TODO: always use OrenNayar ?
			if ( _roughness == 0.f )
			{
				return BRDF::Lambert::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd );
			}
			else
			{
				return BRDF::OrenNayar::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd, _roughness );
			}
		}

	  private:
		Util::Color::Rgba _kd		 = Util::Color::Rgba::WHITE;
		float			  _roughness = 0.f; // [0, 2PI]
	};
} // namespace VTX::App::Old::Render::Renderer::RT::Material

#endif
