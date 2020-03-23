#ifndef __VTX_RENDERER_RAY_TRACER_BASE_PRIMITIVE__
#define __VTX_RENDERER_RAY_TRACER_BASE_PRIMITIVE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "../materials/base_material.hpp"
#include "base_object_3D.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BasePrimitive : public BaseObject3D
		{
		  public:
			BasePrimitive( BaseMaterial * const p_mtl ) : _material( p_mtl ) {}
			virtual ~BasePrimitive() = default;

			const BaseMaterial * getMaterial() const { return _material; }
			const Math::AABB &	 getAABB() const { return _aabb; }

			virtual bool intersect( const Ray &	   p_ray,
									const float	   p_tMin,
									const float	   p_tMax,
									Intersection & p_intersection ) const = 0;

			virtual bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const
			{
				// TODO: add a methode to avoid this Intersection()
				return intersect( p_ray, p_tMin, p_tMax, Intersection( VEC3F_ZERO, VEC3F_ZERO, FLOAT_MAX, nullptr ) );
			}

		  private:
			virtual void _computeAABB() = 0;

		  protected:
			BaseMaterial * _material = nullptr;
		};
	} // namespace Renderer
} // namespace VTX

#endif
