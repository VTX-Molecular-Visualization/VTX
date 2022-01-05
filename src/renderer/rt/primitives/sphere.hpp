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
				const Vec3f & o = p_ray.getOrigin();
				const Vec3f & d = p_ray.getDirection();

				// Use Hearn and Baker ray/sphere intersection equation for small spheres
				// + William et al. more stable quadratic solution
				// See:
				// Haines E., Gunther J., Akenine-Moller T. (2019)
				// Precision Improvements for Ray/Sphere Intersection.
				// In: Haines E., Akenine-Moller T. (eds) Ray Tracing Gems. Apress, Berkeley, CA
				const Vec3f oc	  = o - _center;
				const float b	  = Util::Math::dot( oc, d );
				const float r2	  = _radius * _radius;
				const Vec3f ocd	  = oc - b * d;
				const float delta = r2 - Util::Math::dot( ocd, ocd );

				if ( delta < 0.f )
					return false;

				const float sqrtDelta = sqrtf( delta );

				const float q = ( b > 0.f ) ? -sqrtDelta - b : sqrtDelta - b;

				const float c = Util::Math::dot( oc, oc ) - r2;

				float t = c / q;
				if ( t > p_tMax )
				{
					return false;
				} // first intersection too far
				if ( t < p_tMin )
				{
					t = q;
				} // first intersection too near, check second one
				if ( t < p_tMin || t > p_tMax )
				{
					return false;
				}

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
