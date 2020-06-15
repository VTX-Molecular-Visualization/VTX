#ifndef __VTX_RENDERER_RAY_TRACER_OPTIX_SPHERE__
#define __VTX_RENDERER_RAY_TRACER_OPTIX_SPHERE__

#ifdef _MSC_VER
#pragma once
#endif

#include "intersection.hpp"
#include <optix.h>

namespace VTX::Renderer::Optix
{
	using namespace CUDA; // mandatory to use operator on floatX, etc.

	struct Sphere
	{
		Sphere()  = default;
		~Sphere() = default;

		VTX_INLINE_HOST_DEVICE OptixAabb aabb() const
		{
			return { _center.x - _radius, _center.y - _radius, _center.z - _radius,
					 _center.x + _radius, _center.y + _radius, _center.z + _radius };
		}

		// TODO: uniformize intersects
		VTX_INLINE_HOST_DEVICE bool intersect( const float3 & p_origin,
											   const float3 & p_direction,
											   const float	  p_tMin,
											   const float	  p_tMax,
											   Intersection & p_hit ) const
		{
			const float3 oc		= p_origin - _center;
			const float	 radius = float( _radius );
			const float	 b		= dot( oc, p_direction );
			const float	 c		= dot( oc, oc ) - radius * radius;
			const float	 delta	= b * b - c;

			if ( delta > 0.f )
			{
				const float sqrtDelta = sqrtf( delta );

				float t = ( -b - sqrtDelta );
				if ( t <= p_tMax )
				{ // first intersection not too far
					if ( t < p_tMin )
					{
						t = ( -b + sqrtDelta );
					}								  // first intersection too near, check second one
					if ( t >= p_tMin && t <= p_tMax ) // t is within the interval
					{
						p_hit._t	  = t;
						p_hit._point  = p_origin + p_direction * p_hit._t;
						p_hit._normal = ( p_hit._point - _center ) / radius;

						return true;
					}
				}
			}
			return false;
		}

		float3 _center	= { 0.f, 0.f, 0.f };
		float  _radius	= 0.f;
		int	   _colorId = 0;
	};

} // namespace VTX::Renderer::Optix
#endif
