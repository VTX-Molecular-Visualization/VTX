#ifndef __VTX_RENDERER_DEVICE_MATH_VEC__
#define __VTX_RENDERER_DEVICE_MATH_VEC__

#ifdef _MSC_VER
#pragma once
#endif

#include <cuda.h>

namespace VTX
{
	namespace Renderer
	{
		// ==================================================================== float

		__forceinline__ __device__ float clamp( const float p_f, const float p_min, const float p_max )
		{
			return fmaxf( p_min, fminf( p_f, p_max ) );
		}

		// ==================================================================== float2

		__forceinline__ __device__ float2 operator+( const float2 & p_v1, const float2 & p_v2 )
		{
			return make_float2( p_v1.x + p_v2.x, p_v1.y + p_v2.y );
		}

		__forceinline__ __device__ float2 operator-( const float2 & p_v1, const float2 & p_v2 )
		{
			return make_float2( p_v1.x - p_v2.x, p_v1.y - p_v2.y );
		}

		__forceinline__ __device__ float2 operator+( const float2 & p_v, const float & p_f )
		{
			return make_float2( p_v.x + p_f, p_v.y + p_f );
		}

		__forceinline__ __device__ float2 operator-( const float2 & p_v, const float & p_f )
		{
			return make_float2( p_v.x - p_f, p_v.y - p_f );
		}

		__forceinline__ __device__ float2 operator*( const float2 & p_v, const float & p_f )
		{
			return make_float2( p_v.x * p_f, p_v.y * p_f );
		}

		__forceinline__ __device__ float2 operator/( const float2 & p_v, const float & p_f )
		{
			const float _f = 1.f / p_f;
			return p_v * _f;
		}

		__forceinline__ __device__ float dot( const float2 & p_v1, const float2 & p_v2 )
		{
			return p_v1.x * p_v2.x + p_v1.y * p_v2.y;
		}
		__forceinline__ __device__ float  length( const float2 & v ) { return sqrtf( dot( v, v ) ); }
		__forceinline__ __device__ float2 normalize( const float2 & v ) { return v / length( v ); }

		__forceinline__ __device__ float2 clamp( const float2 & p_v, const float & p_min, const float & p_max )
		{
			return make_float2( clamp( p_v.x, p_min, p_max ), clamp( p_v.y, p_min, p_max ) );
		}

		// ==================================================================== float3

		__forceinline__ __device__ float3 operator+( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( p_v1.x + p_v2.x, p_v1.y + p_v2.y, p_v1.z + p_v2.z );
		}

		__forceinline__ __device__ float3 operator-( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( p_v1.x - p_v2.x, p_v1.y - p_v2.y, p_v1.z - p_v2.z );
		}

		__forceinline__ __device__ float3 operator+( const float3 & p_v, const float & p_f )
		{
			return make_float3( p_v.x + p_f, p_v.y + p_f, p_v.z + p_f );
		}

		__forceinline__ __device__ float3 operator-( const float3 & p_v, const float & p_f )
		{
			return make_float3( p_v.x - p_f, p_v.y - p_f, p_v.z - p_f );
		}

		__forceinline__ __device__ float3 operator*( const float3 & p_v, const float & p_f )
		{
			return make_float3( p_v.x * p_f, p_v.y * p_f, p_v.z * p_f );
		}

		__forceinline__ __device__ float3 operator/( const float3 & p_v, const float & p_f )
		{
			const float _f = 1.f / p_f;
			return p_v * _f;
		}

		__forceinline__ __device__ float dot( const float3 & p_v1, const float3 & p_v2 )
		{
			return p_v1.x * p_v2.x + p_v1.y * p_v2.y + p_v1.z * p_v2.z;
		}
		__forceinline__ __device__ float  length( const float3 & p_v ) { return sqrtf( dot( p_v, p_v ) ); }
		__forceinline__ __device__ float3 normalize( const float3 & p_v ) { return p_v / length( p_v ); }

		__forceinline__ __device__ float3 clamp( const float3 & p_v, const float & p_min, const float & p_max )
		{
			return make_float3(
				clamp( p_v.x, p_min, p_max ), clamp( p_v.y, p_min, p_max ), clamp( p_v.z, p_min, p_max ) );
		}
	} // namespace Renderer
} // namespace VTX

#endif
