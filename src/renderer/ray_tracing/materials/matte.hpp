#ifndef __VTX_RENDERER_RAY_TRACER_MATERIAL_MATTE__
#define __VTX_RENDERER_RAY_TRACER_MATERIAL_MATTE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../brdfs/lambert.hpp"
#include "../brdfs/oren_nayar.hpp"
#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class MatteMaterial : public BaseMaterial
		{
		  public:
			MatteMaterial( const Vec3f & p_kd, const float p_roughness = 0.f ) : _kd( p_kd ), _roughness( p_roughness )
			{
			}
			Vec3f shade( const Ray &		  p_ray,
						 const Intersection & p_hit,
						 const LightSample &  p_lightSample ) const override
			{
				// TODO: always use OrenNayar ?
				if ( _roughness == 0.f )
				{ return BRDF::Lambert::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd ); }
				else
				{
					return BRDF::OrenNayar::fr( p_hit, -p_ray.getDirection(), p_lightSample._dir, _kd, _roughness );
				}
			}

		  private:
			Vec3f _kd		 = VEC3F_XYZ;
			float _roughness = 0.f; // [0, 2PI]
		};
	} // namespace Renderer
} // namespace VTX

#endif
