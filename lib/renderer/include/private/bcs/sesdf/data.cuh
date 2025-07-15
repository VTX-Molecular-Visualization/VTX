#ifndef BCS_SESDF_DATA_CUH
#define BCS_SESDF_DATA_CUH

#include "bcs/cuda/circles.cuh"

namespace bcs::sesdf
{
    struct alignas( 16 ) CircleGeometry
    {
        float3 center;
        float3 normal;
        float  radius;
    };

    struct alignas( 16 ) Circle
    {
        uint32_t i                = 0;
        uint32_t j                = 0;
        uint32_t trimmedId        = 0;
        uint8_t  visibilityStatus = 0;
        uint8_t  intersectionNb   = 0;

        __device__ bool isDuplicated() const { return i >= j; }
        __device__ bool isIntersected() const { return i < j && visibilityStatus == 1; }
    };

    struct alignas( 16 ) SesdfContext
    {
        float probeRadius;

        uint32_t atomNb;

        uint32_t * dIntersectedCircleNb;
        uint32_t * hIntersectedCircleNb;
        uint32_t * dIntersectionNb;
        uint32_t * hIntersectionNb;
        uint32_t * dFullCircleNb;
        uint32_t * hFullCircleNb;
        uint32_t * dSegmentCount;
        uint32_t * hSegmentCount;
        uint32_t * hVisibleCircleNb;

        uint32_t maxNeighborPerAtom;
        uint32_t maxIntersectionsPerCircles;
        uint32_t maxIntersectionNeighbors;

        float4 *   atoms;
        uint32_t * sortedToInitialIndices;

        uint32_t   intersectionWithNeighborNb;
        uint32_t * neighborNb;
        uint32_t * neighborIds;
        uint8_t *  visibilityStatus;
        uint32_t * circlesIntersectionNb;
        uint32_t * circlesIntersectionStartId;
        uint32_t * circlesIntersectionIds;

        uint32_t * trimmedToGlobalId;
        uint32_t * globalToTrimmedId;

        __host__ __device__ uint32_t getMaximumCircleNb() const { return atomNb * maxNeighborPerAtom; }
        __device__ float4            getAtom( const uint32_t atomId ) const { return atoms[ atomId ]; }
        __device__ uint32_t          getNeighborNb( const uint32_t atomId ) const { return neighborNb[ atomId ]; }

        __device__ uint32_t getNeighborId( const uint32_t atomId, const uint16_t localNeighborId ) const
        {
            return neighborIds[ atomId * maxNeighborPerAtom + localNeighborId ];
        }

        __device__ uint32_t globalToTrimmed( const uint32_t atomId, const uint16_t localNeighborId ) const
        {
            return globalToTrimmedId[ atomId * maxNeighborPerAtom + localNeighborId ];
        }

        __device__ uint32_t trimmedToGlobal( const uint32_t trimmedId ) const { return trimmedToGlobalId[ trimmedId ]; }

        __device__ [[nodiscard]] Circle loadCircle( const uint32_t atomId, const uint16_t localNeighborId ) const
        {
            return loadCircle( atomId * maxNeighborPerAtom + localNeighborId );
        }

        __device__ [[nodiscard]] Circle loadCircle( const uint32_t globalId ) const
        {
            Circle circle {};
            circle.i = globalId / maxNeighborPerAtom;
            circle.j = neighborIds[ globalId ];

            circle.visibilityStatus = visibilityStatus[ globalId ];
            if ( circle.isIntersected() )
            {
                circle.trimmedId      = globalToTrimmedId[ globalId ];
                circle.intersectionNb = circlesIntersectionNb[ circle.trimmedId ];
            }

            return circle;
        }

        __device__ [[nodiscard]] CircleGeometry getCircleGeometry( const float4 & ai, const float4 & aj ) const
        {
            const float3 aiPos = make_float3( ai );
            const float3 ajPos = make_float3( aj );

            const float ithExtendedRadius = ai.w + probeRadius;

            CircleGeometry geometry;
            geometry.center        = bcs::getCircleCenter( aiPos, ithExtendedRadius, ajPos, aj.w + probeRadius );
            const float3 circleToI = aiPos - geometry.center;
            const float  squaredCircleDistance = dot( circleToI, circleToI );

            geometry.radius = ::sqrtf( ithExtendedRadius * ithExtendedRadius - squaredCircleDistance );
            geometry.normal = normalize( ajPos - aiPos );

            return geometry;
        }
    };
} // namespace bcs::sesdf

#endif // BCS_SESDF_DATA_CUH
