#ifndef __VTX_RENDERER_RAY_TRACER_OPTIX_SPHERE__
#define __VTX_RENDERER_RAY_TRACER_OPTIX_SPHERE__

#ifdef _MSC_VER
#pragma once
#endif

#include "device_math_vec.hpp"
#include <cuda_runtime.h>
#include <optix.h>

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			// TODO: useful align ?
			// TODO: move from here
			struct __align__( 32 ) Intersection
			{
				float3 _point;
				float  _t;
				float3 _normal;
				float  _padding = 1.f;
			};

			struct __align__( 16 ) Sphere
			{
				Sphere()  = default;
				~Sphere() = default;

				VTX_INLINE_HOST_DEVICE OptixAabb aabb() const
				{
					const float radius = float( _radius );
					return { _center.x - radius, _center.y - radius, _center.z - radius,
							 _center.x + radius, _center.y + radius, _center.z + radius };
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

				float3	 _center  = { 0.f, 0.f, 0.f };
				half	 _radius  = 0.f;
				uint16_t _colorId = 0;
			};
		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX
#endif
