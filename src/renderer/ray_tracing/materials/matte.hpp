#ifndef __VTX_RENDERER_RAY_TRACER_MATERIAL_MATTE__
#define __VTX_RENDERER_RAY_TRACER_MATERIAL_MATTE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../brdfs/lambert.hpp"
#include "base_material.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Renderer
	{
		class MatteMaterial : public BaseMaterial
		{
		  public:
			MatteMaterial( const Vec3f & p_kd ) : _kd( p_kd ) {}
			Vec3f shade( const Ray &		  p_ray,
						 const Intersection & p_hit,
						 const LightSample &  p_lightSample ) const override
			{
				return BRDF::Lambert::fr( p_hit, p_ray.getDirection(), p_lightSample._dir, _kd );
				// TODO add Oren-Nayar if sigma != 0
			}

		  private:
			Vec3f _kd	 = VEC3F_XYZ;
			float _sigma = 0.f;
		};
	} // namespace Renderer
} // namespace VTX

#endif
