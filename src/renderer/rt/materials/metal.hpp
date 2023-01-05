#ifndef __VTX_RENDERER_RAY_TRACER_MATERIAL_METAL__
#define __VTX_RENDERER_RAY_TRACER_MATERIAL_METAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../brdfs/cook_torrance.hpp"
#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class MetalMaterial : public BaseMaterial
		{
		  public:
			MetalMaterial( const Color::Rgba & p_f0, const float p_shininess ) : _f0( p_f0 ), _shininess( p_shininess )
			{
			}

			Color::Rgba getColor() const override { return _f0; }

			Color::Rgba shade( const Ray &			p_ray,
							   const Intersection & p_hit,
							   const LightSample &	p_lightSample ) const override
			{
				// metals don't produce diffuse reflections.
				return BRDF::CookTorrance::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _f0, _shininess );
			}

			static MetalMaterial * createAluminium()
			{
				return new MetalMaterial( Color::Rgba( 0.913f, 0.922f, 0.924f ), 128.f );
			}
			static MetalMaterial * createCopper()
			{
				return new MetalMaterial( Color::Rgba( 0.955f, 0.638f, 0.538f ), 32.f );
			}
			static MetalMaterial * createGold()
			{
				return new MetalMaterial( Color::Rgba( 1.022f, 0.782f, 0.344f ), 64.f );
			}
			static MetalMaterial * createZinc()
			{
				return new MetalMaterial( Color::Rgba( 0.644f, 0.824f, 0.850f ), 64.f );
			}

		  private:
			Color::Rgba _f0		   = Color::Rgba::WHITE;
			float		_shininess = 64.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
