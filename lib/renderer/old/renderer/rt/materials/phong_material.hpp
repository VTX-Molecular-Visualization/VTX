#ifndef __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../brdfs/blinn_phong.hpp"
#include "../brdfs/lambert.hpp"
#include "../brdfs/phong.hpp"
#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class PhongMaterial : public BaseMaterial
		{
		  public:
			PhongMaterial( const Util::Color::Rgba & p_kd, const Util::Color::Rgba & p_ks, const float p_shininess ) :
				_kd( p_kd ), _ks( p_ks ), _shininess( p_shininess )
			{
			}

			Util::Color::Rgba getColor() const override { return _kd; }

			Util::Color::Rgba shade( const Ray &			p_ray,
							   const Intersection & p_hit,
							   const LightSample &	p_lightSample ) const override
			{
				const Util::Color::Rgba diffuse = BRDF::Lambert::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd );
				const Util::Color::Rgba specular
					//= BRDF::Phong::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _ks, _shininess );
					= BRDF::BlinnPhong::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _ks, _shininess );
				return diffuse + specular;
			}

		  private:
			Util::Color::Rgba _kd		   = Util::Color::Rgba::WHITE * 0.5f;
			Util::Color::Rgba _ks		   = Util::Color::Rgba::WHITE * 0.5f;
			float		_shininess = 8.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
