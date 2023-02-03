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
			PhongMaterial( const Color::Rgb & p_kd, const Color::Rgb & p_ks, const float p_shininess ) :
				_kd( p_kd ), _ks( p_ks ), _shininess( p_shininess )
			{
			}

			Color::Rgb getColor() const override { return _kd; }

			Color::Rgb shade( const Ray &		   p_ray,
							  const Intersection & p_hit,
							  const LightSample &  p_lightSample ) const override
			{
				const Color::Rgb diffuse = BRDF::Lambert::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd );
				const Color::Rgb specular
					//= BRDF::Phong::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _ks, _shininess );
					= BRDF::BlinnPhong::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _ks, _shininess );
				return diffuse + specular;
			}

		  private:
			Color::Rgb _kd		  = Color::Rgb::WHITE * 0.5f;
			Color::Rgb _ks		  = Color::Rgb::WHITE * 0.5f;
			float	   _shininess = 8.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
