#ifndef __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_PHONG_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class PhongMaterial : public BaseMaterial
		{
		  public:
			PhongMaterial( const Vec3f & p_color,
						   const Vec3f & p_kd  = VEC3F_XYZ * 0.3f,
						   const Vec3f & p_ks  = VEC3F_XYZ * 0.7f,
						   const float	 p_exp = 10.f ) :
				BaseMaterial( p_color ),
				_kd( p_kd ), _ks( p_ks ), _exp( p_exp )
			{
			}
			Vec3f shade( const Ray & p_ray, const Intersection & p_hit, const Vec3f & p_lightDir ) const override
			{
				const Vec3f reflect = Util::Math::reflect( p_lightDir, p_hit._normal );

				const Vec3f diffuse = _kd * Util::Math::max( 0.f, Util::Math::dot( p_hit._normal, p_lightDir ) );
				const Vec3f specular
					= _ks
					  * Util::Math::pow( Util::Math::max( 0.f, Util::Math::dot( reflect, p_ray.getDirection() ) ),
										 _exp );

				return _color * ( diffuse + specular );
			}

		  private:
			Vec3f _kd  = VEC3F_XYZ * 0.5f;
			Vec3f _ks  = VEC3F_XYZ * 0.5f;
			float _exp = 50.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
