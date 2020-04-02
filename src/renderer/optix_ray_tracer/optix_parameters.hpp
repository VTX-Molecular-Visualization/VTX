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

			// TODO: useful align ?
			struct __align__( 32 ) Intersection
			{
				float3 _point;
				float  _t;
				float3 _normal;
				float  _padding = 1.f;
			};

			// TODO: useful align ?
			struct __align__( 32 ) Sphere
			{
				Sphere() = default;
				Sphere( const float3 & p_center, const float p_radius, const float3 & p_color ) :
					_center( p_center ), _radius( p_radius ), _color( p_color )
				{
				}
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

				float3 _center	= { 0.f, 0.f, 0.f };
				float  _radius	= 0.f;
				float3 _color	= { 0.f, 0.f, 0.f };
				float  _padding = 1.f;
			};

			// TODO: useful align ?
			struct __align__( 64 ) Cylinder
			{
				Cylinder() = default;

				Cylinder( const float3 & p_v0, const float3 & p_v1, const float p_radius ) :
					_v0( p_v0 ), _v1( p_v1 ), _radius( p_radius )
				{
				}

				VTX_INLINE_HOST_DEVICE OptixAabb aabb() const
				{
					const float3 v = _v1 - _v0;
					const float3 e = _radius * sqrt( 1.f - v * v / dot( v, v ) );

					const float3 vMin = minV( _v0 - e, _v1 - e );
					const float3 vMax = maxV( _v0 + e, _v1 + e );

					return { vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z };
				}

				// TODO: uniformize intersects
				VTX_INLINE_HOST_DEVICE bool intersect( const float3 & p_origin,
													   const float3 & p_direction,
													   const float	  p_tMin,
													   const float	  p_tMax,
													   Intersection & p_hit ) const
				{
					const float3 ov0 = p_origin - _v0;
					const float3 v	 = _v1 - _v0;

					const float d0 = dot( v, v );
					const float d1 = dot( v, p_direction );
					const float d2 = dot( v, ov0 );

					const float a = d0 - d1 * d1;
					const float b = d0 * dot( ov0, p_direction ) - d2 * d1;
					const float c = d0 * dot( ov0, ov0 ) - d2 * d2 - _radius * _radius * d0;

					const float h = b * b - a * c;

					if ( h < 0.f ) { return false; }

					float t = ( -b - sqrtf( h ) ) / a;
					if ( t > p_tMax ) { return false; }				   // first intersection too far
					if ( t < p_tMin ) { t = ( -b + sqrtf( h ) ) / a; } // first intersection too near, check second one
					if ( t < p_tMin || t > p_tMax ) { return false; }

					const float y = d2 + t * d1;
					if ( y < 0.f || y > d0 ) { return false; }

					p_hit._t	  = t;
					p_hit._point  = p_origin + p_direction * p_hit._t;
					p_hit._normal = ( ov0 + t * p_direction - v * y / d0 ) / _radius;

					return true;
				}

				float3 _v0;
				float3 _v1;
				float  _radius		 = 0.15f;
				float3 _color		 = { 1.f, 1.f, 1.f };
				float  _padding[ 6 ] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
			};

			struct HitGroupData
			{
				Sphere *   _spheres;
				Cylinder * _cylinders;
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
