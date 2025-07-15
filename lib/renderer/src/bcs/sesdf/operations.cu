#include <cooperative_groups.h>
#include <cub/block/block_scan.cuh>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <thrust/sort.h>
#include <thrust/transform_scan.h>

#include "bcs/cuda/grid.cuh"
#include "bcs/sesdf/data.cuh"
#include "bcs/sesdf/operations.cuh"

namespace bcs::sesdf
{
    void handleIntersectionSingularities( AccelerationGrid &   grid,
                                          const SesdfContext & sesContext,
                                          const float4 * const intersectionsPositions,
                                          int4 *               intersectionAtomIds,
                                          float4 *             intersectionsNeighbors )
    {
        grid.build( sesContext.intersectionWithNeighborNb, intersectionsPositions );
        auto [ numBlocks, numThreads ] = KernelConfig::From( sesContext.intersectionWithNeighborNb, 256 );
        findIntersectionsNeighbors<<<numBlocks, numThreads>>>( grid.getConfiguration(),
                                                               sesContext,
                                                               grid.getSortedPosition(),
                                                               grid.getSortedIndices(),
                                                               grid.getCellsStart(),
                                                               grid.getCellsEnd(),
                                                               intersectionsPositions,
                                                               intersectionAtomIds,
                                                               intersectionsNeighbors );
        cudaCheck( "Find Intersections Neighbors failed" );
    }

    __device__ void findCirclesInCell( const uint32_t       currentAtomIdx,
                                       const float4 &       currentAtomData,
                                       const float4 * const sortedAtomPositions,
                                       const uint32_t       startGridIndex,
                                       const uint32_t       endGridIndex,
                                       const float          probeRadius,
                                       const uint16_t       maxNeighborPerAtom,
                                       uint32_t &           currentNeighborCount,
                                       uint32_t *           neighborsIndices )
    {
        const float  ithExtendedRadius = currentAtomData.w + probeRadius;
        const float3 ithPos            = make_float3( currentAtomData );
        for ( uint32_t j = startGridIndex; j < endGridIndex; j++ )
        {
            if ( currentAtomIdx == j )
                continue;

            const float4 jthAtom           = sortedAtomPositions[ j ];
            const float3 jthPos            = make_float3( jthAtom );
            const float  jthExtendedRadius = jthAtom.w + probeRadius;

            const float dist                = length2( ithPos - jthPos );
            const float atomsExtendedRadius = ithExtendedRadius + jthExtendedRadius;
            if ( dist - atomsExtendedRadius * atomsExtendedRadius <= 1e-4f )
            {
                neighborsIndices[ currentAtomIdx * maxNeighborPerAtom + currentNeighborCount ] = j;
                currentNeighborCount++;

                if ( currentNeighborCount == maxNeighborPerAtom )
                    return;
            }
        }
    }

    __global__ void findCirclesBetweenAtoms( const GridInfo         gridInfo,
                                             const uint32_t * const cellsStart,
                                             const uint32_t * const cellsEnd,
                                             SesdfContext           sesContext )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
        if ( index >= sesContext.atomNb )
            return;

        const float4 currentAtom = sesContext.getAtom( index );
        const int3   gridPos     = gridInfo.getGridPosition( make_float3( currentAtom ) );

        constexpr float MaxVdwRadius  = 3.48f;
        const float     gridCellRange = currentAtom.w + MaxVdwRadius + 2.f * sesContext.probeRadius;
        const int3      cellsInRange  = max( make_int3( ceilf( gridCellRange / gridInfo.cellSize ) ), make_int3( 1 ) );
        const int3      start         = max( gridPos - cellsInRange, make_int3( 0 ) );
        const int3      end           = min( gridPos + cellsInRange, gridInfo.size - 1 );

        uint32_t neighborCount = 0;
        for ( int z = start.z; z <= end.z; z++ )
        {
            for ( int y = start.y; y <= end.y; y++ )
            {
                for ( int x = start.x; x <= end.x; x++ )
                {
                    const int3     currentGridPosition = make_int3( x, y, z );
                    const uint32_t currentGridHash     = gridInfo.getHash( currentGridPosition );
                    const uint32_t startIndex          = cellsStart[ currentGridHash ];

                    if ( startIndex == AccelerationGrid::EmptyGridCellValue )
                        continue;

                    const uint32_t endIndex = cellsEnd[ currentGridHash ];

                    findCirclesInCell( index,
                                       currentAtom,
                                       sesContext.atoms,
                                       startIndex,
                                       endIndex,
                                       sesContext.probeRadius,
                                       sesContext.maxNeighborPerAtom,
                                       neighborCount,
                                       sesContext.neighborIds );
                    if ( neighborCount == sesContext.maxNeighborPerAtom )
                    {
                        printf( "Error: Too many neighbors found for atom %u.\n", blockIdx.x );

                        z = end.z + 1;
                        y = end.y + 1;
                        break;
                    }
                }
            }
        }

        sesContext.neighborNb[ index ] = neighborCount;
    }

    __global__ void computeCirclesVisibilityStatus( SesdfContext sesContext,
                                                    uint2 * __restrict__ fullCirclesData,
                                                    uint32_t * __restrict__ circlesMask )
    {
        extern __shared__ char sharedBuffer[];
        auto *                 blockAtomsData = reinterpret_cast<float4 *>( sharedBuffer );

        float4   blockAtom;
        uint16_t blockAtomCircleNb;
        uint32_t j;
        float4   jthData;
        if ( blockIdx.x < sesContext.atomNb )
        {
            blockAtom         = sesContext.getAtom( blockIdx.x );
            blockAtomCircleNb = sesContext.getNeighborNb( blockIdx.x );
            if ( threadIdx.x < blockAtomCircleNb )
            {
                j       = sesContext.getNeighborId( blockIdx.x, threadIdx.x );
                jthData = sesContext.getAtom( j );

                blockAtomsData[ threadIdx.x ] = jthData;
            }
        }

        cg::this_thread_block().sync();

        const uint32_t globalCircleIdx = blockIdx.x * sesContext.maxNeighborPerAtom + threadIdx.x;
        if ( threadIdx.x >= blockAtomCircleNb )
        {
            sesContext.visibilityStatus[ globalCircleIdx ] = 0;
            return;
        }

        uint32_t isVisible  = threadIdx.x < blockAtomCircleNb;
        uint32_t isComplete = threadIdx.x < blockAtomCircleNb;

        // Check if current circle is complete and not entirely occluded
        const CircleGeometry circle = sesContext.getCircleGeometry( blockAtom, jthData );
        for ( uint16_t kCircleIndex = 0; kCircleIndex < blockAtomCircleNb; kCircleIndex++ )
        {
            if ( threadIdx.x == kCircleIndex )
                continue;

            const float4 kData             = blockAtomsData[ kCircleIndex ];
            const float  kthExtendedRadius = kData.w + sesContext.probeRadius;

            const float3 kToCircle    = circle.center - make_float3( kData );
            const float  distToCircle = length( kToCircle );

            const float secondDistance = dot( kToCircle, circle.normal );
            const float cosTheta       = secondDistance / distToCircle;

            // Check if current circle is entirely covered by an atom
            const float sinTheta = ::sqrtf( 1.f - cosTheta * cosTheta );
            {
                const float firstDistance    = sinTheta * distToCircle + circle.radius;
                const float completeDistance = firstDistance * firstDistance + secondDistance * secondDistance;
                if ( completeDistance - kthExtendedRadius * kthExtendedRadius < 1e-4f )
                {
                    isVisible = 0;
                    break;
                }
            }

            if ( isComplete )
            {
                const float firstDistance    = ( -sinTheta ) * distToCircle + circle.radius;
                const float completeDistance = firstDistance * firstDistance + secondDistance * secondDistance;
                if ( completeDistance - kthExtendedRadius * kthExtendedRadius < 1e-4f )
                    isComplete = 0;
            }
        }

        sesContext.visibilityStatus[ globalCircleIdx ] = isVisible * ( 1 + isComplete );
        if ( isVisible == 1 && isComplete == 1 && j > blockIdx.x )
            fullCirclesData[::atomicAdd( sesContext.dFullCircleNb, 1 ) ] = make_uint2( blockIdx.x, j );

        circlesMask[ globalCircleIdx ] = static_cast<uint32_t>( !isComplete && isVisible && j > blockIdx.x );
    }

    __global__ void fillIntersections( SesdfContext     sesContext,
                                       const uint32_t * circleIntersectionStartIds,
                                       int4 *           intersectionAtomIds,
                                       uint32_t *       stencil,
                                       uint32_t *       circleIntersectionIds,
                                       float4 *         intersectionsPositions )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
        if ( index >= *sesContext.dIntersectionNb )
            return;

        const int4 trimmedIds = intersectionAtomIds[ index ];

        const uint32_t globalIj = sesContext.trimmedToGlobal( trimmedIds.x );
        const uint32_t globalIk = sesContext.trimmedToGlobal( trimmedIds.z );

        const uint32_t i = globalIj / sesContext.maxNeighborPerAtom;
        const uint32_t j
            = sesContext.getNeighborId( i, static_cast<uint16_t>( globalIj - i * sesContext.maxNeighborPerAtom ) );
        const uint32_t k
            = sesContext.getNeighborId( i, static_cast<uint16_t>( globalIk - i * sesContext.maxNeighborPerAtom ) );

        const float4 ai    = sesContext.getAtom( i );
        const float3 aiPos = make_float3( ai );
        float        sqR2  = ai.w + sesContext.probeRadius;
        sqR2 *= sqR2;

        const float4         aj = sesContext.getAtom( j );
        const CircleGeometry ij = sesContext.getCircleGeometry( ai, aj );

        const float4         ak = sesContext.getAtom( k );
        const CircleGeometry ik = sesContext.getCircleGeometry( ai, ak );

        const float3 u = ik.normal - dot( ik.normal, ij.normal ) * ij.normal;
        const float  t = dot( ik.center - ij.center, ik.normal ) / dot( u, ik.normal );

        const float3 x1              = ij.center + t * u;
        const float  squareIToX1Dist = length2( x1 - aiPos );

        const float c = fmax( -squareIToX1Dist + sqR2, 0.f );

        const float3 n            = normalize( cross( ij.normal, ik.normal ) );
        const float3 intersection = x1 + ( sign( static_cast<float>( trimmedIds.w ) ) * ::sqrtf( c ) * n );

        intersectionsPositions[ index ] = make_float4( intersection, trimmedIds.w );

        uint32_t currentIntId = ::atomicAdd( stencil + trimmedIds.x, 1 );
        assert( currentIntId < sesContext.circlesIntersectionNb[ trimmedIds.x ] );

        uint32_t writingPosition                 = circleIntersectionStartIds[ trimmedIds.x ] + currentIntId;
        circleIntersectionIds[ writingPosition ] = index;

        currentIntId = ::atomicAdd( stencil + trimmedIds.y, 1 );
        assert( currentIntId < sesContext.circlesIntersectionNb[ trimmedIds.y ] );
        writingPosition                          = circleIntersectionStartIds[ trimmedIds.y ] + currentIntId;
        circleIntersectionIds[ writingPosition ] = index;

        currentIntId = ::atomicAdd( stencil + trimmedIds.z, 1 );
        assert( currentIntId < sesContext.circlesIntersectionNb[ trimmedIds.z ] );
        writingPosition                          = circleIntersectionStartIds[ trimmedIds.z ] + currentIntId;
        circleIntersectionIds[ writingPosition ] = index;

        intersectionAtomIds[ index ] = make_int4( i, j, k, 0 );
    }

    __global__ void findIntersectionsNeighbors( const GridInfo         gridInfo,
                                                SesdfContext           sesContext,
                                                const float4 * const   sortedIntersectionPos,
                                                const uint32_t * const sortedIntersectionIds,
                                                const uint32_t * const cellsStart,
                                                const uint32_t * const cellsEnd,
                                                const float4 * const   intersectionPositions,
                                                int4 *                 intersectionAtomIds,
                                                float4 *               intersectionNeighbors )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;

        if ( index >= sesContext.intersectionWithNeighborNb )
            return;

        const uint32_t sortedIdx  = sortedIntersectionIds[ index ];
        const float3   currentPos = make_float3( sortedIntersectionPos[ index ] );
        const int3     gridPos    = gridInfo.getGridPosition( currentPos );

        const float gridCellRange = 2.f * sesContext.probeRadius;
        const int3  cellsInRange
            = max( make_int3( ceilf( make_float3( gridCellRange ) / gridInfo.cellSize ) ), make_int3( 1 ) );

        const int3 start = max( gridPos - cellsInRange, make_int3( 0 ) );
        const int3 end   = min( gridPos + cellsInRange, gridInfo.size - 1 );

        uint32_t neighborCount = 0;
        for ( int z = start.z; z <= end.z; z++ )
        {
            for ( int y = start.y; y <= end.y; y++ )
            {
                for ( int x = start.x; x <= end.x; x++ )
                {
                    const int3     currentGridPosition = make_int3( x, y, z );
                    const uint32_t currentGridHash     = gridInfo.getHash( currentGridPosition );
                    const uint32_t startIndex          = cellsStart[ currentGridHash ];

                    if ( startIndex == AccelerationGrid::EmptyGridCellValue )
                        continue;

                    const uint32_t endIndex = cellsEnd[ currentGridHash ];
                    for ( uint32_t j = startIndex; j < endIndex; j++ )
                    {
                        if ( index == j )
                            continue;

                        const float3 jthPos      = make_float3( sortedIntersectionPos[ j ] );
                        const float  currentDist = length( currentPos - jthPos );
                        if ( currentDist < gridCellRange )
                        {
                            intersectionNeighbors[ sortedIdx * sesContext.maxIntersectionNeighbors + neighborCount ]
                                = make_float4( jthPos, currentPos.x * currentPos.y * currentPos.z );

                            neighborCount++;
                            if ( neighborCount == sesContext.maxIntersectionNeighbors )
                            {
                                z = end.z + 1;
                                y = end.y + 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        intersectionAtomIds[ sortedIdx ].w = static_cast<int>( neighborCount );
    }
} // namespace bcs::sesdf
