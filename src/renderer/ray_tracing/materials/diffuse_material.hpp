#ifndef __VTX_RENDERER_RAY_TRACER_DIFFUSE_MATERIAL__
#define __VTX_RENDERER_RAY_TRACER_DIFFUSE_MATERIAL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class DiffuseMaterial : public BaseMaterial
		{
		  public:
			DiffuseMaterial( const Vec3f & p_color, const Vec3f & p_kd = VEC3F_XYZ ) :
				BaseMaterial( p_color ), _kd( p_kd )
			{
			}
			Vec3f shade( const Ray & p_ray, const Intersection & p_hit, const Vec3f & p_lightDir ) const override
			{
				return _color * _kd * Util::Math::max( 0.f, Util::Math::dot( p_hit._normal, p_lightDir ) );
			}

		  private:
			Vec3f _kd = VEC3F_XYZ;
		};
	} // namespace Renderer
} // namespace VTX

#endif
