#ifndef __VTX_RENDERER_DEVICE_MATH_VEC__
#define __VTX_RENDERER_DEVICE_MATH_VEC__

#ifdef _MSC_VER
#pragma once
#endif

//#ifdef OPTIX_DEFINED

#include "cuda/common.hpp"

namespace VTX
{
	namespace CUDA
	{
		// ==================================================================== float

		VTX_INLINE_HOST_DEVICE float clamp( const float p_f, const float p_min, const float p_max )
		{
			return fmaxf( p_min, fminf( p_f, p_max ) );
		}

		// ==================================================================== float2

		VTX_INLINE_HOST_DEVICE float2 operator+( const float2 & p_v1, const float2 & p_v2 )
		{
			return make_float2( p_v1.x + p_v2.x, p_v1.y + p_v2.y );
		}

		VTX_INLINE_HOST_DEVICE float2 operator-( const float2 & p_v1, const float2 & p_v2 )
		{
			return make_float2( p_v1.x - p_v2.x, p_v1.y - p_v2.y );
		}

		VTX_INLINE_HOST_DEVICE float2 operator+( const float2 & p_v, const float p_f )
		{
			return make_float2( p_v.x + p_f, p_v.y + p_f );
		}

		VTX_INLINE_HOST_DEVICE float2 operator-( const float2 & p_v, const float p_f )
		{
			return make_float2( p_v.x - p_f, p_v.y - p_f );
		}

		VTX_INLINE_HOST_DEVICE float2 operator*( const float2 & p_v, const float p_f )
		{
			return make_float2( p_v.x * p_f, p_v.y * p_f );
		}

		VTX_INLINE_HOST_DEVICE float2 operator/( const float2 & p_v, const float p_f )
		{
			const float _f = 1.f / p_f;
			return p_v * _f;
		}

		VTX_INLINE_HOST_DEVICE float dot( const float2 & p_v1, const float2 & p_v2 )
		{
			return p_v1.x * p_v2.x + p_v1.y * p_v2.y;
		}
		VTX_INLINE_HOST_DEVICE float  length( const float2 & v ) { return sqrtf( dot( v, v ) ); }
		VTX_INLINE_HOST_DEVICE float2 normalize( const float2 & v ) { return v / length( v ); }

		VTX_INLINE_HOST_DEVICE float2 clamp( const float2 & p_v, const float p_min, const float p_max )
		{
			return make_float2( clamp( p_v.x, p_min, p_max ), clamp( p_v.y, p_min, p_max ) );
		}

		// ==================================================================== float3

		VTX_INLINE_HOST_DEVICE float3 operator+( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( p_v1.x + p_v2.x, p_v1.y + p_v2.y, p_v1.z + p_v2.z );
		}
		VTX_INLINE_HOST_DEVICE float3 operator+( const float3 & p_v, const float p_f )
		{
			return make_float3( p_v.x + p_f, p_v.y + p_f, p_v.z + p_f );
		}
		VTX_INLINE_HOST_DEVICE float3 operator+( const float p_f, const float3 p_v ) { return p_v + p_f; }

		VTX_INLINE_HOST_DEVICE float3 & operator+=( const float3 & p_v1, const float3 & p_v2 ) { return p_v1 + p_v2; }

		VTX_INLINE_HOST_DEVICE float3 operator-( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( p_v1.x - p_v2.x, p_v1.y - p_v2.y, p_v1.z - p_v2.z );
		}
		VTX_INLINE_HOST_DEVICE float3 operator-( const float3 & p_v, const float p_f )
		{
			return make_float3( p_v.x - p_f, p_v.y - p_f, p_v.z - p_f );
		}
		VTX_INLINE_HOST_DEVICE float3 operator-( const float p_f, const float3 & p_v )
		{
			return make_float3( p_f - p_v.x, p_f - p_v.y, p_f - p_v.z );
		}

		VTX_INLINE_HOST_DEVICE float3 operator*( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( p_v1.x * p_v2.x, p_v1.y * p_v2.y, p_v1.z * p_v2.z );
		}
		VTX_INLINE_HOST_DEVICE float3 operator*( const float3 & p_v, const float p_f )
		{
			return make_float3( p_v.x * p_f, p_v.y * p_f, p_v.z * p_f );
		}
		VTX_INLINE_HOST_DEVICE float3 operator*( const float p_f, const float3 & p_v ) { return p_v * p_f; }
		VTX_INLINE_HOST_DEVICE float3 & operator*=( const float3 & p_v1, const float3 & p_v2 ) { return p_v1 * p_v2; }
		VTX_INLINE_HOST_DEVICE float3 & operator*=( const float3 & p_v1, const float & p_f ) { return p_v1 * p_f; }

		VTX_INLINE_HOST_DEVICE float3 operator/( const float3 & p_v, const float p_f )
		{
			const float _f = 1.f / p_f;
			return p_v * _f;
		}

		VTX_INLINE_HOST_DEVICE float3 minV( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( fminf( p_v1.x, p_v2.x ), fminf( p_v1.y, p_v2.y ), fminf( p_v1.z, p_v2.z ) );
		}
		VTX_INLINE_HOST_DEVICE float3 maxV( const float3 & p_v1, const float3 & p_v2 )
		{
			return make_float3( fmaxf( p_v1.x, p_v2.x ), fmaxf( p_v1.y, p_v2.y ), fmaxf( p_v1.z, p_v2.z ) );
		}

		VTX_INLINE_HOST_DEVICE float3 sqrt( const float3 & p_v )
		{
			return make_float3( sqrtf( p_v.x ), sqrtf( p_v.y ), sqrtf( p_v.z ) );
		}

		VTX_INLINE_HOST_DEVICE float dot( const float3 & p_v1, const float3 & p_v2 )
		{
			return p_v1.x * p_v2.x + p_v1.y * p_v2.y + p_v1.z * p_v2.z;
		}
		VTX_INLINE_HOST_DEVICE float  length( const float3 & p_v ) { return sqrtf( dot( p_v, p_v ) ); }
		VTX_INLINE_HOST_DEVICE float3 normalize( const float3 & p_v ) { return p_v / length( p_v ); }

		VTX_INLINE_HOST_DEVICE float3 clamp( const float3 & p_v, const float p_min, const float p_max )
		{
			return make_float3(
				clamp( p_v.x, p_min, p_max ), clamp( p_v.y, p_min, p_max ), clamp( p_v.z, p_min, p_max ) );
		}
	} // namespace CUDA
} // namespace VTX

//#endif
#endif
