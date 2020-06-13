#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

//#ifndef OPTIX_DEFINED

#include "device_math_vec.hpp"
#include "sphere.hpp"
#include <cuda_runtime.h>
#include <optix.h>
#include <stdint.h>

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			enum
			{
				RADIANCE_RAY_TYPE = 0,
				SHADOW_RAY_TYPE,
				RAY_TYPE_COUNT
			};

			struct __align__( 32 ) LaunchParameters
			{
				struct
				{
					uchar4 * _pixels = nullptr;
					uint16_t _width	 = 0;
					uint16_t _height = 0;
					uint32_t _id	 = 0;
				} _frame;

				float3 * _colors;

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

			// TODO: useful align ?
			struct __align__( 32 ) Cylinder
			{
				Cylinder() = default;

				VTX_INLINE_HOST_DEVICE OptixAabb aabb() const
				{
					const float3 v = _v1 - _v0;
					const float3 e = _radius * sqrt( 1.f - v * v / dot( v, v ) );

					const float3 vMin = minV( _v0 - e, _v1 - e );
					const float3 vMax = maxV( _v0 + e, _v1 + e );

					return { vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z };
				}

				// TODO: uniformize intersects
				// VTX_INLINE_HOST_DEVICE bool intersect( const float3 & p_origin,
				//									   const float3 & p_direction,
				//									   const float	  p_tMin,
				//									   const float	  p_tMax,
				//									   Intersection & p_hit ) const
				//{
				//	const float3 ov0 = p_origin - _v0;
				//	const float3 v	 = _v1 - _v0;

				//	const float d0 = dot( v, v );
				//	const float d1 = dot( v, p_direction );
				//	const float d2 = dot( v, ov0 );

				//	const float a = d0 - d1 * d1;
				//	const float b = d0 * dot( ov0, p_direction ) - d2 * d1;
				//	const float c = d0 * dot( ov0, ov0 ) - d2 * d2 - _radius * _radius * d0;

				//	const float h = b * b - a * c;

				//	if ( h < 0.f )
				//	{
				//		return false;
				//	}

				//	float t = ( -b - sqrtf( h ) ) / a;
				//	if ( t > p_tMax )
				//	{
				//		return false;
				//	} // first intersection too far
				//	if ( t < p_tMin )
				//	{
				//		t = ( -b + sqrtf( h ) ) / a;
				//	} // first intersection too near, check second one
				//	if ( t < p_tMin || t > p_tMax )
				//	{
				//		return false;
				//	}

				//	const float y = d2 + t * d1;
				//	if ( y < 0.f || y > d0 )
				//	{
				//		return false;
				//	}

				//	p_hit._t	  = t;
				//	p_hit._point  = p_origin + p_direction * p_hit._t;
				//	p_hit._normal = ( ov0 + t * p_direction - v * y / d0 ) / _radius;

				//	return true;
				//}

				float3	 _v0;
				float3	 _v1;
				float	 _radius  = 0.15f;
				uint16_t _colorId = 0;
				uint16_t _padding;
			};

			struct HitGroupData
			{
				Optix::Sphere * _spheres;
				Cylinder *		_cylinders;
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

//#endif
#endif
