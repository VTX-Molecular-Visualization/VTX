#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

#include "device_math_vec.hpp"
#include <cuda_runtime.h>
#include <optix.h>
#include <stdint.h>

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			struct LaunchParameters
			{
				struct
				{
					uchar4 * _pixels = nullptr;
					uint32_t _width	 = 0;
					uint32_t _height = 0;
					uint32_t _id	 = 0;
				} _frame;

				OptixTraversableHandle _traversable = 0ull;
			};

			struct RayGeneratorData
			{
				struct
				{
					float3 _position;
					float3 _front;
					float3 _du;
					float3 _dv;
				} _camera;
			};

			struct MissData
			{
				float3 _colorBackground;
			};

			struct __align__( 32 ) Intersection
			{
				float3 _point;
				float  _t;
				float3 _normal;
				float  _padding = 1.f;
			};

			struct __align__( 32 ) Sphere
			{
				float3 _center;
				float  _radius;
				float3 _color;
				float  _padding = 1.f;

				Sphere() = default;
				Sphere( const float3 & p_center, const float p_radius, const float3 & p_color ) :
					_center( p_center ), _radius( p_radius ), _color( p_color )
				{
				}
				~Sphere() = default;

				OptixAabb getAABB() const
				{
					return { _center.x - _radius, _center.y - _radius, _center.z - _radius,
							 _center.x + _radius, _center.y + _radius, _center.z + _radius };
				}

				VTX_INLINE_HOST_DEVICE bool intersect( const float3 & p_origin,
													   const float3 & p_direction,
													   const float	  p_tMin,
													   const float	  p_tMax,
													   Intersection & p_hit ) const
				{
					const float3 oc	   = p_origin - _center;
					const float	 b	   = dot( oc, p_direction );
					const float	 c	   = dot( oc, oc ) - _radius * _radius;
					const float	 delta = b * b - c;

					if ( delta > 0.f )
					{
						const float sqrtDelta = sqrtf( delta );

						float t = ( -b - sqrtDelta );
						if ( t <= p_tMax )
						{ // first intersection not too far
							if ( t < p_tMin )
							{ t = ( -b + sqrtDelta ); }		  // first intersection too near, check second one
							if ( t >= p_tMin && t <= p_tMax ) // t is within the interval
							{
								p_hit._t	  = t;
								p_hit._point  = p_origin + p_direction * p_hit._t;
								p_hit._normal = ( p_hit._point - _center ) / _radius;

								return true;
							}
						}
					}
					return false;
				}
			};

			struct HitGroupData
			{
				Sphere * _spheres;
				/*float3 * _positions;
				float *	 _radii;
				float3 * _colors;*/
			};

			template<typename T>
			struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) Record
			{
				__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

				T _data;
			};

			using RayGeneratorRecord = Record<Optix::RayGeneratorData>;
			using MissRecord		 = Record<Optix::MissData>;
			using HitGroupRecord	 = Record<Optix::HitGroupData>;
		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX

#endif
