#ifndef BCS_CUDA_CIRCLES_CUH
#define BCS_CUDA_CIRCLES_CUH

#include "bcs/cuda/math.cuh"

namespace bcs
{
    // Reference: https://github.com/quanchaoyu/MolSurfComp/blob/master/Core/data_I_Cir.m#L255
    inline __device__ float3 getCircleCenter( const float3 c1, const float r1, const float3 c2, float r2 )
    {
        const float d = length2( c1 - c2 );
        const float t = ( r1 * r1 - r2 * r2 + d ) / ( 2.f * d );
        return c1 + ( c2 - c1 ) * t;
    }

    // Compute small circle on the vdW sphere based on the circle between two SAS spheres
    inline __device__ float4 getSmallCircle( const float4 & atom, const float3 & n, const float3 & x )
    {
        const float3 aPos             = make_float3( atom );
        const float3 sCircle          = aPos + n * dot( n, normalize( x - aPos ) * atom.w );
        const float  sqCircleDistance = length2( aPos - sCircle );
        const float  r                = ::sqrtf( atom.w * atom.w - sqCircleDistance );
        return make_float4( sCircle, r );
    }

    // Reference: https://github.com/quanchaoyu/MolSurfComp/blob/master/Dist_Computation/fsas.m#L204
    inline __device__ float angleBetweenEdges( const float3 u, const float3 v, const float3 n )
    {
        const float theta = acosf( clamp( dot( u, v ) / ( length( u ) * length( v ) ), -1.f, 1.f ) );
        return sign( determinant( u, v, n ) ) > 0.f ? theta : bcs::TwoPif - theta;
    }
} // namespace bcs

#endif // BCS_CUDA_CIRCLES_CUH
