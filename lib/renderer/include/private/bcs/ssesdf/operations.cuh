#ifndef BCS_SSESDF_OPERATIONS_CUH
#define BCS_SSESDF_OPERATIONS_CUH

#include "bcs/cuda/grid.cuh"
#include "bcs/cuda/memory.cuh"
#include "bcs/ssesdf/data.cuh"
#include "bcs/ssesdf/graphics.hpp"

namespace bcs::ssesdf
{
    void findCircles( AccelerationGrid & grid, SsesdfContext & sesContext );

    template<uint32_t MaxNeighborPerAtom>
    std::pair<ResultBuffer, ResultBuffer> findIntersections( SsesdfContext & sesContext );

    void handleIntersectionSingularities( AccelerationGrid &    grid,
                                          const SsesdfContext & sesContext,
                                          float4 *              intersectionsPositions,
                                          int4 *                intersectionAtomIds,
                                          float4 *              intersectionsNeighbors );

    __global__ void findCirclesBetweenAtoms( const GridInfo         gridInfo,
                                             const uint32_t * const cellsStart,
                                             const uint32_t * const cellsEnd,
                                             SsesdfContext          sesContext );

    __global__ void computeCirclesVisibilityStatus( SsesdfContext sesContext );

    __global__ void fillCircles( SsesdfContext          sesContext,
                                 uint32_t               visibleCircleNb,
                                 const uint32_t * const trimmedToGlobalId,
                                 float4 *               positions,
                                 float4 *               axis,
                                 float4 *               visibilitySpheres );

    template<uint32_t MaxNeighborPerAtom>
    __global__ void findIntersectionsBetweenCircles( SsesdfContext sesContext, int4 * intersectionAtomIds );

    __global__ void fillIntersections( SsesdfContext sesContext,
                                       int4 *        intersectionAtomIds,
                                       float4 *      intersectionsPositions );

    __global__ void findIntersectionsNeighbors( const GridInfo         gridInfo,
                                                SsesdfContext          sesContext,
                                                const float4 * const   sortedIntersectionPos,
                                                const uint32_t * const sortedIntersectionIds,
                                                const uint32_t * const cellsStart,
                                                const uint32_t * const cellsEnd,
                                                 float4 *    intersectionPositions,
                                                int4 *                 intersectionAtomIds,
                                                float4 *               intersectionNeighbors );
} // namespace bcs::ssesdf

#include "bcs/ssesdf/operation.inl"

#endif // BCS_SSESDF_OPERATIONS_CUH