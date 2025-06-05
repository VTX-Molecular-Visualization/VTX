#ifndef BCS_SESDF_OPERATIONS_CUH
#define BCS_SESDF_OPERATIONS_CUH

#include "bcs/cuda/grid.cuh"
#include "bcs/cuda/memory.cuh"
#include "bcs/sesdf/data.cuh"
#include "bcs/sesdf/graphics.hpp"

namespace bcs::sesdf
{
    template<uint32_t MaxNeighborPerAtom>
    std::pair<ResultBuffer, DeviceBuffer> findCircles( AccelerationGrid & grid,
                                                       SesdfContext &     sesContext,
                                                       uint2 *            convexPatches,
                                                       bool               graphics );

    template<uint32_t MaxNeighborPerAtom>
    std::pair<ResultBuffer, DeviceBuffer> findIntersections( SesdfContext & sesContext, bool graphics );

    template<uint8_t MaxIntersectionsPerCircle>
    ResultBuffer buildConicPatches( SesdfContext &       sesContext,
                                    const float4 * const intersectionsPositions,
                                    const int4 * const   intersectionAtomIds, bool graphics );

    void handleIntersectionSingularities( AccelerationGrid &   grid,
                                          const SesdfContext & sesContext,
                                          const float4 * const intersectionsPositions,
                                          int4 *               intersectionAtomIds,
                                          float4 *             intersectionsNeighbors );

    __global__ void findCirclesBetweenAtoms( const GridInfo         gridInfo,
                                             const uint32_t * const cellsStart,
                                             const uint32_t * const cellsEnd,
                                             SesdfContext           sesContext );

    __global__ void computeCirclesVisibilityStatus( SesdfContext sesContext,
                                                    uint2 * __restrict__ fullCirclesData,
                                                    uint32_t * __restrict__ circlesMask );

    template<uint32_t MaxNeighborPerAtom>
    __global__ void findIntersectionsBetweenCircles( SesdfContext sesContext, int4 * intersectionAtomIds );

    __global__ void fillIntersections( SesdfContext     sesContext,
                                       const uint32_t * circleIntersectionStartIds,
                                       int4 *           intersectionAtomIds,
                                       uint32_t *       stencil,
                                       uint32_t *       circleIntersectionIds,
                                       float4 *         intersectionsPositions );

    template<uint16_t BlockThreadNb, uint16_t MaxIntersectionPerCircles>
    __global__ void buildSegments( const SesdfContext     sesContext,
                                   const uint32_t         circleWithSegmentNb,
                                   const uint32_t *       circleWithSegmentIndices,
                                   const float4 * const   intersectionPositions,
                                   const int4 * const     intersectionAtomIds,
                                   const uint32_t * const segmentsIndices,
                                   uint4 *                segmentsData );

    template<uint32_t MaxNeighborPerAtom>
    __global__ void buildConvexPatchesSectors( SesdfContext           sesContext,
                                               const uint32_t * const unoccludedCirclesScan,
                                               uint2 *                atomsElementIds,
                                               float4 *               sectors );

    __global__ void findIntersectionsNeighbors( const GridInfo         gridInfo,
                                                SesdfContext           sesContext,
                                                const float4 * const   sortedIntersectionPos,
                                                const uint32_t * const sortedIntersectionIds,
                                                const uint32_t * const cellsStart,
                                                const uint32_t * const cellsEnd,
                                                const float4 * const   intersectionPositions,
                                                int4 *                 intersectionAtomIds,
                                                float4 *               intersectionNeighbors );
} // namespace bcs::sesdf

#include "bcs/sesdf/operations.inl"

#endif // BCS_SESDF_OPERATIONS_CUH