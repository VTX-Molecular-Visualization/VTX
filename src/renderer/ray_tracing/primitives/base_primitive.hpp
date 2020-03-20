#ifndef __VTX_RENDERER_RAY_TRACER_BASE_PRIMITIVE__
#define __VTX_RENDERER_RAY_TRACER_BASE_PRIMITIVE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "../materials/base_material.hpp"
#include "../ray.hpp"
#include "math/aabb.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BasePrimitive
		{
		  public:
			BasePrimitive( BaseMaterial * const p_mtl ) : _material( p_mtl ) {}
			virtual ~BasePrimitive() = default;

			const BaseMaterial * getMaterial() const { return _material; }
			const Math::AABB &	 getAABB() const { return _aabb; }

			virtual void computeAABB() = 0;

			virtual bool intersect( const Ray &	   p_ray,
									const float	   p_tMin,
									const float	   p_tMax,
									Intersection & p_intersection ) const = 0;

		  protected:
			BaseMaterial * _material = nullptr;
			Math::AABB	   _aabb;
		};
	} // namespace Renderer
} // namespace VTX

#endif
