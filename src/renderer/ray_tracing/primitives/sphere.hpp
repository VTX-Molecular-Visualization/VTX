#ifndef __VTX_RENDERER_RAY_TRACER_SPHERE__
#define __VTX_RENDERER_RAY_TRACER_SPHERE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_primitive.hpp"

namespace VTX
{
	namespace Renderer
	{
		class Sphere : public BasePrimitive
		{
		  public:
			~Sphere() = default;
			Sphere( const Vec3f & p_center, const float p_radius, BaseMaterial * const p_mtl ) :
				BasePrimitive( p_mtl ), _center( p_center ), _radius( p_radius )
			{
				_computeAABB();
			}

			const Vec3f & centroid() const { return _center; }
			const float	  radius() const { return _radius; }

			Vec3f getVecMin() const { return _center - _radius; }
			Vec3f getVecMax() const { return _center + _radius; }

			bool intersect( const Ray &	   p_ray,
							const float	   p_tMin,
							const float	   p_tMax,
							Intersection & p_intersection ) const override
			{
				const Vec3f & o		= p_ray.getOrigin();
				const Vec3f & d		= p_ray.getDirection();
				const Vec3f	  oc	= o - _center;
				const float	  b		= Util::Math::dot( oc, d );
				const float	  c		= Util::Math::dot( oc, oc ) - _radius * _radius;
				const float	  delta = b * b - c;

				if ( delta < 0.f ) return false;

				const float sqrtDelta = sqrtf( delta );

				float t = -b - sqrtDelta;
				if ( t < p_tMin || t > p_tMax ) { return false; }

				p_intersection._point	  = p_ray.getPointAtT( t );
				const Vec3f normal		  = ( p_intersection._point - _center ) / _radius;
				p_intersection._normal	  = Util::Math::faceForward( normal, d );
				p_intersection._distance  = t;
				p_intersection._primitive = this;

				return true;
			}

		  private:
			void _computeAABB() override { _aabb = Math::AABB( getVecMin(), getVecMax() ); }

		  private:
			Vec3f _center = VEC3F_ZERO;
			float _radius = 1.f;
		};
	} // namespace Renderer
} // namespace VTX
#endif
