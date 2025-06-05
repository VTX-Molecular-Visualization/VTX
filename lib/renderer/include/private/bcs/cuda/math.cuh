#ifndef BCS_CUDA_MATH_CUH
#define BCS_CUDA_MATH_CUH

#include <cmath>

#include <device_types.h>
#include <helper_math.h>

#include "bcs/core/math.hpp"

namespace bcs
{
    inline __host__ __device__ float sign( float f )
    {
        return copysignf(1.f, f);
    }

    inline __host__ __device__ float length2( const float2 & v ) { return dot( v, v ); }
    inline __host__ __device__ float length2( const float3 & v ) { return dot( v, v ); }
    inline __host__ __device__ float length2( const float4 & v ) { return dot( v, v ); }

    inline __host__ __device__ float3 ceilf( float3 a )
    {
        return make_float3( ::ceilf( a.x ),  ::ceilf( a.y ), ::ceilf( a.z ) );
    }

    inline __host__ __device__ float determinant( const float3 a, const float3 b, const float3 c )
    {
        return dot( a, cross( b, c ) );
    }

    inline __host__ __device__ float3 orthogonalVector( const float3 normal )
    {
        float3 ref = { 1.f, 0.f, 0.f };
        if ( fabsf( dot( normal, ref ) ) > 1.f - 1e-4f )
            ref = make_float3( 0.f, 1.f, 0.f );
        return normalize( cross( normal, ref ) );
    }

} // namespace bcs

#endif // BCS_CUDA_MATH_CUH
