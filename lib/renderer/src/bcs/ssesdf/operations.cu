#include <cub/block/block_scan.cuh>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <thrust/sort.h>
#include <thrust/transform_scan.h>

#include "bcs/cuda/grid.cuh"
#include "bcs/ssesdf/data.cuh"
#include "bcs/ssesdf/operations.cuh"

namespace bcs::ssesdf
{
    template<class Type>
    struct IsNonZero : thrust::unary_function<Type, Type>
    {
        __host__ __device__ Type operator()( Type x ) { return static_cast<Type>( x != 0 ); }
    };

    template<class Type>
    struct IsIntersected : thrust::unary_function<Type, Type>
    {
        __host__ __device__ Type operator()( Type x ) { return static_cast<Type>( x == 1 ); }
    };

    void findCircles( AccelerationGrid & grid, SsesdfContext & sesContext )
    {
        {
            grid.build( sesContext.atomNb, sesContext.atoms );

            copy( sesContext.atoms, grid.getSortedPosition(), sesContext.atomNb );
            copy( sesContext.sortedToInitialIndices, grid.getSortedIndices(), sesContext.atomNb );

            auto [ numBlocks, numThreads ] = KernelConfig::From( sesContext.atomNb, sesContext.maxNeighborPerAtom );
            findCirclesBetweenAtoms<<<numBlocks, numThreads>>>(
                grid.getConfiguration(), grid.getCellsStart(), grid.getCellsEnd(), sesContext );
            cudaCheck( "Circles evaluation failed" );
        }

        computeCirclesVisibilityStatus<<<sesContext.atomNb,
                                         sesContext.maxNeighborPerAtom,
                                         sesContext.maxNeighborPerAtom * sizeof( float4 )>>>( sesContext );
        cudaCheck( "Circles visibility evaluation failed" );

        *sesContext.hVisibleCircleNb
            = thrust::count_if( thrust::device,
                                sesContext.visibilityStatus,
                                sesContext.visibilityStatus + sesContext.atomNb * sesContext.maxNeighborPerAtom,
                                IsNonZero<uint32_t>() );

        sesContext.intersectedCircleNb
            = thrust::count_if( thrust::device,
                                sesContext.visibilityStatus,
                                sesContext.visibilityStatus + sesContext.atomNb * sesContext.maxNeighborPerAtom,
                                IsNonZero<uint32_t>() );
    }

    void handleIntersectionSingularities( AccelerationGrid &    grid,
                                          const SsesdfContext & sesContext,
                                          float4 *              intersectionsPositions,
                                          int4 *                intersectionAtomIds,
                                          float4 *              intersectionsNeighbors )
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
                                             SsesdfContext          sesContext )
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

    __global__ void computeCirclesVisibilityStatus( SsesdfContext sesContext )
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

        uint32_t isVisible  = threadIdx.x < blockAtomCircleNb && blockIdx.x < j;
        uint32_t isComplete = threadIdx.x < blockAtomCircleNb && blockIdx.x < j;

        // Check if current circle is complete and not entirely occluded
        if ( isVisible )
        {
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
        }
    }

    __global__ void fillCircles( SsesdfContext          sesContext,
                                 uint32_t               visibleCircleNb,
                                 const uint32_t * const trimmedToGlobalId,
                                 float4 *               positions,
                                 float4 *               axis,
                                 float4 *               visibilitySpheres )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
        if ( index >= visibleCircleNb )
            return;

        const uint32_t globalId = trimmedToGlobalId[ index ];

        const Circle         ij     = sesContext.loadCircle( globalId );
        const float4         ai     = sesContext.getAtom( ij.i );
        const float4         aj     = sesContext.getAtom( ij.j );
        const CircleGeometry circle = sesContext.getCircleGeometry( ai, aj );

        // torus axis
        float3 ta = normalize( circle.center - make_float3( ai ) );
        // torus center
        float3 tc    = circle.center;
        float3 ortho = normalize( cross( ta, make_float3( 0.0f, 0.0f, 1.0f ) ) );

        // compute the tangential point X2 of the spheres
        float3 P = tc + ( ortho * circle.radius );
        float3 X = normalize( P - make_float3( ai ) ) * ai.w;
        float3 C = ( length( P - make_float3( ai ) )
                     / ( length( P - make_float3( aj ) ) + length( P - make_float3( ai ) ) ) )
                   * ( make_float3( aj ) - make_float3( ai ) );
        float distance = length( X - C );
        C              = ( C + make_float3( ai ) ) - tc;

        // write torus center & torus radius R
        positions[ index ] = make_float4( tc, circle.radius );
        // write torus axis & probe radius (= torus radius r)
        axis[ index ] = make_float4( ta, sesContext.probeRadius );
        // write visibility sphere
        visibilitySpheres[ index ] = make_float4( C, distance );
    }

    __global__ void fillIntersections( SsesdfContext sesContext,
                                       int4 *        intersectionAtomIds,
                                       float4 *      intersectionsPositions )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
        if ( index >= *sesContext.dIntersectionNb )
            return;

        const int4 circlesIds = intersectionAtomIds[ index ];

        const uint32_t i = circlesIds.x / sesContext.maxNeighborPerAtom;
        const uint32_t j = sesContext.neighborIds[ circlesIds.x ];
        const uint32_t k = sesContext.neighborIds[ circlesIds.y ];

        atomicCAS( sesContext.visibilityStatus + circlesIds.x, 1, 2 );
        atomicCAS( sesContext.visibilityStatus + circlesIds.y, 1, 2 );
        atomicCAS( sesContext.visibilityStatus + circlesIds.z, 1, 2 );

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
        const float3 intersection = x1 + ( sign( static_cast<float>( circlesIds.w ) ) * sqrtf( c ) * n );

        intersectionsPositions[ index ] = make_float4( intersection, circlesIds.w );
        intersectionAtomIds[ index ]    = make_int4( i, j, k, 0 );
    }

    __global__ void findIntersectionsNeighbors( const GridInfo         gridInfo,
                                                SsesdfContext          sesContext,
                                                const float4 * const   sortedIntersectionPos,
                                                const uint32_t * const sortedIntersectionIds,
                                                const uint32_t * const cellsStart,
                                                const uint32_t * const cellsEnd,
                                                float4 *               intersectionPositions,
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
                                = make_float4( jthPos, 0.f );

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

        intersectionPositions[ sortedIdx ].w = static_cast<int>( sortedIdx * sesContext.maxIntersectionNeighbors );
        intersectionAtomIds[ sortedIdx ].w   = static_cast<int>( neighborCount );
    }
} // namespace bcs::ssesdf
