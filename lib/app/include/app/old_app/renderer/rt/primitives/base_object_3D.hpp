#ifndef __VTX_RENDERER_RAY_TRACER_BASE_OBJECT3D__
#define __VTX_RENDERER_RAY_TRACER_BASE_OBJECT3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "../intersection.hpp"
#include "../ray.hpp"
#include "app/component/object3d/helper/aabb.hpp"

namespace VTX
{
	namespace Renderer
	{
		class BaseObject3D
		{
		  public:
			BaseObject3D()			= default;
			virtual ~BaseObject3D() = default;

			const App::Component::Object3D::Helper::AABB & getAABB() const { return _aabb; }

			virtual bool intersect( const Ray &	   p_ray,
									const float	   p_tMin,
									const float	   p_tMax,
									Intersection & p_intersection ) const
				= 0;

			virtual bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const = 0;

		  protected:
		 App::Component::Object3D::Helper::AABB _aabb;

		  private:
			virtual void _computeAABB() = 0;
		};
	} // namespace Renderer
} // namespace VTX

#endif
