#ifndef __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

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
			PhongMaterial( const Vec3f & p_kd		 = VEC3F_XYZ * 0.4f,
						   const Vec3f & p_ks		 = VEC3F_XYZ * 0.6f,
						   const float	 p_shininess = 8.f ) :
				_kd( p_kd ),
				_ks( p_ks ), _shininess( p_shininess )
			{
			}
			Vec3f shade( const Ray &		  p_ray,
						 const Intersection & p_hit,
						 const LightSample &  p_lightSample ) const override
			{
				const Vec3f diffuse = BRDF::Lambert::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd );
				const Vec3f specular
					= BRDF::Phong::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _ks, _shininess );

				return diffuse + specular;
			}

		  private:
			Vec3f _kd		 = VEC3F_XYZ * 0.5f;
			Vec3f _ks		 = VEC3F_XYZ * 0.5f;
			float _shininess = 8.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
