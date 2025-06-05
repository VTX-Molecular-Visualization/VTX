#pragma executioncharacterset( "utf-8" )

#include "bcs/cuda/execution.cuh"
#include "bcs/sesdf/operations.cuh"
#include <cooperative_groups.h>
#include <cub/warp/warp_merge_sort.cuh>
#include <thrust/count.h>
#include <thrust/execution_policy.h>
#include <thrust/sort.h>

namespace bcs::sesdf
{
	namespace cg = cooperative_groups;

	template<uint32_t MaxNeighborPerAtom>
	std::pair<ResultBuffer, DeviceBuffer> findCircles(
		AccelerationGrid & grid,
		SesdfContext &	   sesContext,
		uint2 *			   convexPatches,
		bool			   graphics
	)
	{
		{
			grid.build( sesContext.atomNb, sesContext.atoms );

			copy( sesContext.atoms, grid.getSortedPosition(), sesContext.atomNb );
			copy( sesContext.sortedToInitialIndices, grid.getSortedIndices(), sesContext.atomNb );

			auto [ numBlocks, numThreads ] = KernelConfig::From( sesContext.atomNb, sesContext.maxNeighborPerAtom );
			findCirclesBetweenAtoms<<<numBlocks, numThreads>>>(
				grid.getConfiguration(), grid.getCellsStart(), grid.getCellsEnd(), sesContext
			);
			cudaCheck( "Circles evaluation failed" );
		}

		// Circles visibility status
		DeviceBuffer temp { sesContext.getMaximumCircleNb() * sizeof( uint2 )
								+ ( sesContext.getMaximumCircleNb() + 1 ) * sizeof( uint32_t ),
							true };

		uint32_t * mask
			= reinterpret_cast<uint32_t *>( temp.get<uint8_t>() + sesContext.getMaximumCircleNb() * sizeof( uint2 ) );
		uint2 * tempFullCircles = temp.get<uint2>();
		computeCirclesVisibilityStatus<<<
			sesContext.atomNb,
			sesContext.maxNeighborPerAtom,
			sesContext.maxNeighborPerAtom * sizeof( float4 )>>>( sesContext, tempFullCircles, mask );
		cudaCheck( "Circles visibility evaluation failed" );

		prefixSumCount(
			mask, sesContext.globalToTrimmedId, sesContext.hIntersectedCircleNb, sesContext.getMaximumCircleNb() + 1
		);

		auto trimmedToGlobalId = DeviceBuffer::Typed<uint32_t>( *sesContext.hIntersectedCircleNb );
		thrust::copy_if(
			thrust::device,
			thrust::make_counting_iterator<uint32_t>( 0 ),
			thrust::make_counting_iterator<uint32_t>( sesContext.getMaximumCircleNb() ),
			mask,
			trimmedToGlobalId.get<uint32_t>(),
			thrust::identity<uint32_t>()
		);

		// Convex patches
		thrust::transform_exclusive_scan(
			thrust::device,
			sesContext.visibilityStatus,
			sesContext.visibilityStatus + sesContext.getMaximumCircleNb() + 1,
			mask,
			[] __device__( uint8_t x ) { return uint32_t( x != 0 ); },
			0,
			thrust::plus<uint32_t>()
		);

		mmemcpy<MemcpyType::DeviceToHost>( sesContext.hVisibleCircleNb, mask + sesContext.getMaximumCircleNb(), 1 );
		const uint32_t visibleCirclesNb = *sesContext.hVisibleCircleNb;

		mmemcpy<MemcpyType::DeviceToHost>( sesContext.hFullCircleNb, sesContext.dFullCircleNb, 1 );

		const std::size_t alignment = ( *sesContext.hFullCircleNb * sizeof( uint2 ) ) % sizeof( float4 );
		ResultBuffer	  fCirclesAndSectors { *sesContext.hFullCircleNb * sizeof( uint2 ) + alignment
											   + visibleCirclesNb * sizeof( float4 ),
										   false,
										   graphics };

		if ( visibleCirclesNb > 0 )
		{
			buildConvexPatchesSectors<MaxNeighborPerAtom><<<sesContext.atomNb, sesContext.maxNeighborPerAtom>>>(
				sesContext,
				mask,
				convexPatches,
				fCirclesAndSectors.get<float4>( *sesContext.hFullCircleNb * sizeof( uint2 ) + alignment )
			);
			cudaCheck( "Convex patch construction failed" );
		}

		if ( *sesContext.hFullCircleNb > 0 )
			copy( fCirclesAndSectors.get<uint2>(), tempFullCircles, *sesContext.hFullCircleNb );

		return std::make_pair<ResultBuffer, DeviceBuffer>(
			std::move( fCirclesAndSectors ), std::move( trimmedToGlobalId )
		);
	}

	struct HasNeighborsComparator
	{
		// "If f is an object of class greater<T> and x and y are objects of class T, then f(x,y) returns true if x > y
		// and false otherwise."
		__device__ bool operator()( const int4 a, const int4 b ) { return abs( a.w ) > abs( b.w ); }
	};
	struct HasNeighbors
	{
		__device__ bool operator()( const int4 a ) { return abs( a.w ) == 2; }
	};

	template<uint32_t MaxNeighborPerAtom>
	std::pair<ResultBuffer, DeviceBuffer> findIntersections( SesdfContext & sesContext, bool graphics )
	{
		ResultBuffer intersectionBuffer;
		{
			auto tempIntersectionsIds = DeviceBuffer::Typed<int4>(
				( *sesContext.hIntersectedCircleNb ) * sesContext.maxIntersectionsPerCircles
			);

			findIntersectionsBetweenCircles<MaxNeighborPerAtom>
				<<<sesContext.atomNb, sesContext.maxNeighborPerAtom>>>( sesContext, tempIntersectionsIds.get<int4>() );
			cudaCheck( "Intersection construction failed" );

			mmemcpy<MemcpyType::DeviceToHost>( sesContext.hIntersectionNb, sesContext.dIntersectionNb, 1 );

			if ( *sesContext.hIntersectionNb
				 > ( *sesContext.hIntersectedCircleNb ) * sesContext.maxIntersectionsPerCircles )
			{
				fmt::print(
					"Error: More intersection than expected",
					*sesContext.hIntersectionNb,
					( *sesContext.hIntersectedCircleNb ) * sesContext.maxIntersectionsPerCircles
				);
			}

			// Place with-neighbors intersections at the begin of the array
			int4 * intersectionIds = tempIntersectionsIds.get<int4>();
			thrust::sort(
				thrust::device, intersectionIds, intersectionIds + *sesContext.hIntersectionNb, HasNeighborsComparator()
			);
			sesContext.intersectionWithNeighborNb = thrust::count_if(
				thrust::device, intersectionIds, intersectionIds + *sesContext.hIntersectionNb, HasNeighbors()
			);
			intersectionBuffer = ResultBuffer(
				( *sesContext.hIntersectionNb ) * ( sizeof( int4 ) + sizeof( float4 ) )
					+ sesContext.intersectionWithNeighborNb * sizeof( float4 ) * sesContext.maxIntersectionNeighbors,
				false,
				graphics
			);
			copy( intersectionBuffer.get<int4>(), tempIntersectionsIds.get<int4>(), *sesContext.hIntersectionNb );
		}

		const uint32_t circleIntersectionNb = ( *sesContext.hIntersectionNb ) * 3;
		auto		   temp = DeviceBuffer::Typed<uint32_t>( *sesContext.hIntersectedCircleNb + circleIntersectionNb );
		uint32_t *	   startIds = temp.get<uint32_t>();
		uint32_t *	   ids		= temp.get<uint32_t>() + *sesContext.hIntersectedCircleNb;
		{
			thrust::transform_exclusive_scan(
				thrust::device,
				sesContext.circlesIntersectionNb,
				sesContext.circlesIntersectionNb + ( *sesContext.hIntersectedCircleNb ),
				startIds,
				[] __device__( uint8_t c ) { return static_cast<uint32_t>( c ); },
				0,
				thrust::plus<uint32_t>()
			);

			auto circleIntersectionStencil = DeviceBuffer::Typed<uint32_t>( *sesContext.hIntersectedCircleNb, true );

			auto [ blockNb, threadNb ] = KernelConfig::From( *sesContext.hIntersectionNb, 256 );
			fillIntersections<<<blockNb, threadNb>>>(
				sesContext,
				startIds,
				intersectionBuffer.get<int4>(),
				circleIntersectionStencil.get<uint32_t>(),
				ids,
				reinterpret_cast<float4 *>(
					intersectionBuffer.get<uint8_t>() + sizeof( int4 ) * ( *sesContext.hIntersectionNb )
				)
			);
			cudaCheck( "Intersection fill failed" );
		}

		return std::make_pair<ResultBuffer, DeviceBuffer>( std::move( intersectionBuffer ), std::move( temp ) );
	}

	template<uint32_t MaxNeighborPerAtom>
	__global__ void buildConvexPatchesSectors(
		SesdfContext		   sesContext,
		const uint32_t * const unoccludedCirclesScan,
		uint2 *				   atomsElementIds,
		float4 *			   sectors
	)
	{
		__shared__ uint32_t writeStartIndex;

		using BlockScan = cub::BlockScan<uint32_t, MaxNeighborPerAtom>;
		__shared__ typename BlockScan::TempStorage tempStorage;

		const uint32_t circleIndex = blockIdx.x * blockDim.x + threadIdx.x;
		const uint8_t  status	   = sesContext.visibilityStatus[ circleIndex ];
		const uint32_t isSector	   = status != 0;

		float4 sector;
		if ( isSector )
		{
			const float4 ai = sesContext.getAtom( blockIdx.x );
			const float3 ci = make_float3( ai );

			const float rirp   = ai.w + sesContext.probeRadius;
			const float sqrirp = rirp * rirp;

			const uint32_t j  = sesContext.neighborIds[ circleIndex ];
			const float4   aj = sesContext.getAtom( j );
			const float3   cj = make_float3( aj );

			const float3 cij = getCircleCenter( ci, rirp, cj, aj.w + sesContext.probeRadius );

			const float3 cijci	 = ci - cij;
			const float	 sqcijci = dot( cijci, cijci );
			const float	 radius	 = ::sqrtf( sqrirp - sqcijci );

			const float3 cicj = normalize( cj - ci );
			const float3 n	  = orthogonalVector( cicj );
			const float3 x	  = cij + n * radius;

			sector = make_float4( cicj, acosf( clamp( dot( cicj, ( x - ci ) / rirp ), -1.f, 1.f ) ) );
		}

		if ( threadIdx.x == 0 )
			writeStartIndex = unoccludedCirclesScan[ blockIdx.x * sesContext.maxNeighborPerAtom ];

		__syncthreads();

		uint32_t writingIdx;
		BlockScan( tempStorage ).ExclusiveSum( isSector, writingIdx );
		if ( isSector )
			sectors[ writeStartIndex + writingIdx ] = sector;

		if ( threadIdx.x == blockDim.x - 1 )
			atomsElementIds[ blockIdx.x ] = make_uint2( writeStartIndex, writeStartIndex + writingIdx + isSector );
	}

	template<uint32_t MaxNeighborPerAtom>
	__global__ void findIntersectionsBetweenCircles( SesdfContext sesContext, int4 * intersectionAtomIds )
	{
		constexpr uint32_t WarpThreadNb	 = 32;
		constexpr uint32_t BlockThreadNb = MaxNeighborPerAtom;
		static_assert(
			MaxNeighborPerAtom % WarpThreadNb == 0,
			"MaxNeighborPerAtom must be a multiple of the number of warp threads."
		);

		constexpr uint32_t WarpNb		= BlockThreadNb / WarpThreadNb;
		constexpr uint32_t HiddenCircle = 0xffffffff;

		__shared__ uint32_t blockNeighborhoodIndices[ BlockThreadNb ];
		__shared__ float4	blockNeighborhoodData[ BlockThreadNb ];

		__shared__ uint32_t trimmedCircleNb;
		__shared__ uint32_t trimmedCircles[ BlockThreadNb ];

		__shared__ uint32_t groupIntersectionNb;
		__shared__ int4		groupIntersectionsIds[ BlockThreadNb ];

		__shared__ uint32_t startIntersectionId;

		auto workGroup = cg::this_thread_block();

		if ( threadIdx.x == 0 )
			groupIntersectionNb = startIntersectionId = trimmedCircleNb = 0;

		workGroup.sync();

		const float4   ai		   = sesContext.getAtom( blockIdx.x );
		const uint32_t iNeighborNb = sesContext.getNeighborNb( blockIdx.x );
		if ( threadIdx.x < iNeighborNb )
		{
			const Circle circle = sesContext.loadCircle( blockIdx.x, threadIdx.x );
			const float4 aj		= sesContext.getAtom( circle.j );

			blockNeighborhoodData[ threadIdx.x ] = aj;
			if ( circle.isIntersected() )
			{
				blockNeighborhoodIndices[ threadIdx.x ]				= circle.j;
				trimmedCircles[::atomicAdd( &trimmedCircleNb, 1 ) ] = threadIdx.x;
			}
			else
			{
				blockNeighborhoodIndices[ threadIdx.x ] = HiddenCircle;
			}
		}

		workGroup.sync();

		auto  warp		  = cg::tiled_partition<WarpThreadNb>( workGroup );
		float aiSqERadius = ai.w + sesContext.probeRadius;
		aiSqERadius *= aiSqERadius;

		uint32_t neighborSave[ MaxNeighborPerAtom / WarpThreadNb ];
		for ( uint32_t warpTask = warp.meta_group_rank(); warpTask < trimmedCircleNb; warpTask += WarpNb )
		{
			const uint32_t localNeighbor = trimmedCircles[ warpTask ];

			Circle		   ij;
			float4		   aj;
			uint16_t	   jNeighborNb;
			CircleGeometry circle;
			if ( warp.thread_rank() == 0 )
			{
				ij			= sesContext.loadCircle( blockIdx.x, localNeighbor );
				aj			= sesContext.getAtom( ij.j );
				jNeighborNb = sesContext.getNeighborNb( ij.j );
				circle		= sesContext.getCircleGeometry( ai, aj );
			}

			ij			= warp.shfl( ij, 0 );
			aj			= warp.shfl( aj, 0 );
			jNeighborNb = warp.shfl( jNeighborNb, 0 );
			circle		= warp.shfl( circle, 0 );

			// Load j's neighborhood into warp memory
			const uint32_t neighborsPerThread = sesContext.maxNeighborPerAtom / WarpThreadNb;
			assert( neighborsPerThread == MaxNeighborPerAtom / WarpThreadNb );
			for ( uint32_t i = 0; i < neighborsPerThread; i++ )
			{
				const uint32_t jLocalNeighbor = i * warp.size() + warp.thread_rank();
				neighborSave[ i ]			  = sesContext.getNeighborId( ij.j, jLocalNeighbor );
			}

			// Process j's neighborhood
			const uint32_t iterationNb = std::ceil( static_cast<float>( iNeighborNb ) / warp.size() );
			for ( uint32_t iteration = 0; iteration < iterationNb; iteration++ )
			{
				// Check if another circle of i can create an intersection with ij
				uint32_t	   k;
				uint32_t	   mustBeProcessed = 0;
				const uint32_t localK		   = iteration * warp.size() + warp.thread_rank();
				if ( localK < iNeighborNb )
				{
					k				= blockNeighborhoodIndices[ localK ];
					mustBeProcessed = k != HiddenCircle && ij.j < k;
				}

				// For all ik which can create an intersection, the warp check if k is a circle of j
				uint32_t allowedToContinue = 0;
				uint32_t jkIndex		   = 0xffffffff;
				executeQueue<WarpThreadNb>( // format
                    warp.ballot( mustBeProcessed ),
                    [ & ]( uint32_t id )
                    {
                        const uint32_t currentK = warp.shfl( k, id );

                        // Each thread of the warp check if its set of loaded j's neighbors contain k
                        //                                k
                        //                              / |
                        //                            /   |       with i < j < k
                        //       We just found      /     |
                        //       this edge    --> /       |
                        //                      /         | <- We want to make sure if this one exist
                        //                    /           |
                        //                  /             |
                        //                /               |
                        //              i _______________ j
                        //                      ^
                        //                      |
                        //               We're processing
                        //                   this edge

                        uint32_t jKExist     = 0;
                        uint32_t tempJKIndex = 0xffffffff;
                        for ( uint32_t i = 0; !jKExist && i < neighborsPerThread; i++ )
                        {
                            const uint32_t index = i * warp.size() + warp.thread_rank();
                            jKExist              = index < jNeighborNb && neighborSave[ i ] == currentK;
                            if ( jKExist )
                                tempJKIndex = index;

                            jKExist = warp.ballot( jKExist );
                        }

                        // If a thread find k in its set, the corresponding thread save its value
                        if ( jKExist )
                        {
                            tempJKIndex = warp.shfl( tempJKIndex, getIdFromMask<WarpThreadNb>( jKExist ) );
                            if ( warp.thread_rank() == id )
                            {
                                jkIndex           = tempJKIndex;
                                allowedToContinue = true;
                            }
                        }
                    } );

				// All IK previously validated now compute their true intersection data
				float3 x1, x2;
				float4 ak;
				if ( allowedToContinue )
				{
					ak							  = blockNeighborhoodData[ localK ];
					const CircleGeometry ikCircle = sesContext.getCircleGeometry( ai, ak );

					const float3 u = ikCircle.normal - dot( ikCircle.normal, circle.normal ) * circle.normal;

					const float3 xc
						= circle.center
						  + u * dot( ikCircle.center - circle.center, ikCircle.normal ) / dot( u, ikCircle.normal );
					const float t	  = -length2( xc - make_float3( ai ) ) + aiSqERadius;
					allowedToContinue = t >= 0.f;
					if ( allowedToContinue )
					{
						const float3 n = ::sqrtf( t ) * normalize( cross( ikCircle.normal, circle.normal ) );
						x1			   = xc - n;
						x2			   = xc + n;
					}
				}

				// For every intersection found, the warp check if it isn't hidden by a neighbor of i
				executeQueue<WarpThreadNb>( // format
                    warp.ballot( allowedToContinue ),
                    [ & ]( uint32_t id )
                    {
                        // Load ijk triplet from the corresponding thread
                        const uint16_t iterationLocalK  = iteration * warp.size() + id;
                        const uint32_t iterationKjIndex = warp.shfl( jkIndex, id );

                        const float4 iterationAk = warp.shfl( ak, id );
                        const float3 x1ToProcess = warp.shfl( x1, id );
                        const float3 x2ToProcess = warp.shfl( x2, id );

                        // Check intersection visibility status
                        bool x1Visible = true;
                        bool x2Visible = true;
                        for ( uint16_t lIndex = warp.thread_rank(); ( x1Visible || x2Visible ) && lIndex < iNeighborNb;
                              lIndex += warp.size() )
                        {
                            if ( lIndex == localNeighbor || lIndex == iterationLocalK )
                                continue;

                            const float4 secondAtom        = blockNeighborhoodData[ lIndex ];
                            const float3 secondAtomPos     = make_float3( secondAtom );
                            float        lthExtendedRadius = secondAtom.w + sesContext.probeRadius;
                            lthExtendedRadius *= lthExtendedRadius;

                            x1Visible = x1Visible && length2( secondAtomPos - x1ToProcess ) - lthExtendedRadius > 0.f;
                            x2Visible = x2Visible && length2( secondAtomPos - x2ToProcess ) - lthExtendedRadius > 0.f;
                        }

                        // First thread save intersections if they are visible
                        x1Visible = warp.all( x1Visible );
                        x2Visible = warp.all( x2Visible );
                        if ( warp.thread_rank() == 0 && ( x1Visible || x2Visible ) )
                        {
                            const uint32_t trimmedJk = sesContext.globalToTrimmed( ij.j, iterationKjIndex );
                            const uint32_t trimmedIk = sesContext.globalToTrimmed( ij.i, iterationLocalK );

                            const int3   ids  = make_int3( ij.trimmedId, trimmedJk, trimmedIk );
                            const float3 p2   = make_float3( ai );
                            const float3 p3   = make_float3( aj );
                            const float3 p4   = make_float3( iterationAk );
                            const float3 p2p3 = normalize( p3 - p2 );
                            const float3 p4p3 = normalize( p3 - p4 );

                            if ( x1Visible )
                            {
                                const float    d = dot( p2 - x1ToProcess, normalize( cross( p2p3, p4p3 ) ) );
                                const uint32_t hasNeighbors
                                    = d * d - sesContext.probeRadius * sesContext.probeRadius < -1e-4f;

                                groupIntersectionsIds[::atomicAdd( &groupIntersectionNb, 1 ) ]
                                    = make_int4( ids, ( 1 + hasNeighbors ) );
                            }
                            if ( x2Visible )
                            {
                                const float    d = dot( p2 - x2ToProcess, normalize( cross( p2p3, p4p3 ) ) );
                                const uint32_t hasNeighbors
                                    = d * d - sesContext.probeRadius * sesContext.probeRadius < -1e-4f;

                                groupIntersectionsIds[::atomicAdd( &groupIntersectionNb, 1 ) ]
                                    = make_int4( ids, -( 1 + hasNeighbors ) );
                            }
                        }
                    } );
			}
		}

		// Saving intersections data to global memory
		workGroup.sync();
		if ( workGroup.thread_rank() == 0 )
			startIntersectionId = ::atomicAdd( sesContext.dIntersectionNb, groupIntersectionNb );
		workGroup.sync();

		for ( uint32_t i = workGroup.thread_rank(); i < groupIntersectionNb; i += workGroup.size() )
		{
			const int4 current = groupIntersectionsIds[ i ];

			::atomicAdd( &sesContext.circlesIntersectionNb[ current.x ], 1 );
			::atomicAdd( &sesContext.circlesIntersectionNb[ current.y ], 1 );
			::atomicAdd( &sesContext.circlesIntersectionNb[ current.z ], 1 );

			intersectionAtomIds[ startIntersectionId + i ] = current;
		}
	}

	template<uint8_t MaxIntersectionsPerCircle>
	ResultBuffer buildConicPatches(
		SesdfContext &		 sesContext,
		const float4 * const intersectionsPositions,
		const int4 * const	 intersectionAtomIds,
		bool				 graphics
	)
	{
		// Extract circles with segment's ids and segments number from circles status
		const std::size_t buffer1Size = *sesContext.hIntersectedCircleNb + 1;
		auto			  temp = DeviceBuffer::Typed<uint32_t>( buffer1Size + *sesContext.hIntersectedCircleNb, true );
		uint32_t *		  segmentIds			= temp.get<uint32_t>();
		uint32_t *		  circleWithSegmentsIds = temp.get<uint32_t>() + buffer1Size;

		thrust::transform(
			thrust::device,
			sesContext.circlesIntersectionNb,
			sesContext.circlesIntersectionNb + ( *sesContext.hIntersectedCircleNb ),
			segmentIds,
			[] __device__( uint8_t intersectionNb ) { return intersectionNb / 2u; }
		);

		uint32_t * lastIndex = thrust::copy_if(
			thrust::device,
			thrust::make_counting_iterator<uint32_t>( 0 ),
			thrust::make_counting_iterator<uint32_t>( *sesContext.hIntersectedCircleNb ),
			segmentIds,
			circleWithSegmentsIds,
			[] __device__( uint32_t x ) { return static_cast<uint32_t>( x != 0 ); }
		);

		const uint32_t circleWithSegmentNb = std::distance( circleWithSegmentsIds, lastIndex );

		// The sum provides indices for direct saving during segments extraction
		prefixSumCount( segmentIds, segmentIds, sesContext.hSegmentCount, buffer1Size );

		ResultBuffer segmentsDataBuffer;
		if ( *sesContext.hSegmentCount > 0 )
		{
			segmentsDataBuffer = ResultBuffer::Typed<uint4>( *sesContext.hSegmentCount, false, graphics );

			auto [ numBlocks, numThreads ] = KernelConfig::From( circleWithSegmentNb, 256 );

			constexpr uint32_t WarpThreadNb = 16;
			if ( numThreads.x % WarpThreadNb != 0 )
				numThreads.x = circleWithSegmentNb + WarpThreadNb - circleWithSegmentNb % WarpThreadNb;

			buildSegments<256, MaxIntersectionsPerCircle><<<numBlocks, numThreads>>>(
				sesContext,
				circleWithSegmentNb,
				circleWithSegmentsIds,
				intersectionsPositions,
				intersectionAtomIds,
				segmentIds,
				segmentsDataBuffer.get<uint4>()
			);
			cudaCheck( "Segments construction failed" );
		}

		return std::move( segmentsDataBuffer );
	}

	struct LessOperator
	{
		template<typename DataType>
		__device__ bool operator()( const DataType & lhs, const DataType & rhs )
		{
			return lhs < rhs;
		}
	};

	template<uint16_t BlockThreadNb, uint16_t MaxIntersectionPerCircles>
	__global__ void buildSegments(
		const SesdfContext	   sesContext,
		const uint32_t		   circleWithSegmentNb,
		const uint32_t *	   circleWithSegmentIndices,
		const float4 * const   intersectionPositions,
		const int4 * const	   intersectionAtomIds,
		const uint32_t * const segmentsIndices,
		uint4 *				   segmentsData
	)
	{
		namespace cg = cooperative_groups;

		constexpr uint16_t WarpThreadNb = 16;
		constexpr uint16_t WarpPerBLock = BlockThreadNb / WarpThreadNb;
		using WarpMergeSortT			= cub::WarpMergeSort<float, MaxIntersectionPerCircles, WarpThreadNb, uint32_t>;
		__shared__ typename WarpMergeSortT::TempStorage tempStorage[ WarpPerBLock ];

		// Per thread buffers holding intersections data
		float	 circleAngles[ MaxIntersectionPerCircles ];
		uint32_t circleIntersectionIds[ MaxIntersectionPerCircles ];
		uint32_t intersectionIds[ MaxIntersectionPerCircles ];

		// Buffers data initialization
		constexpr float AngleOffset = 2.f * 3.14f + 1.f;

		const float startAngle = static_cast<float>( threadIdx.x ) * AngleOffset;
		const float initAngle  = static_cast<float>( threadIdx.x + 1 ) * AngleOffset;
		for ( uint16_t i = 0; i < MaxIntersectionPerCircles; i++ )
		{
			circleAngles[ i ]		   = initAngle;
			circleIntersectionIds[ i ] = MaxIntersectionPerCircles - 1;
		}

		// Per circle data
		uint32_t trimmedId;
		float	 pickedDirection;
		Circle	 circle;

		const uint32_t circleWithSegmentId = blockIdx.x * blockDim.x + threadIdx.x;
		if ( circleWithSegmentId < circleWithSegmentNb )
		{
			// Load current circle data
			trimmedId					   = circleWithSegmentIndices[ circleWithSegmentId ];
			const uint32_t globalCircleIdx = sesContext.trimmedToGlobal( trimmedId );
			circle						   = sesContext.loadCircle( globalCircleIdx );

			assert( circle.intersectionNb < MaxIntersectionPerCircles );
			circle.intersectionNb = min( circle.intersectionNb, MaxIntersectionPerCircles - 1 );
			assert( circle.intersectionNb > 0 );

			const float4		 ai						  = sesContext.getAtom( circle.i );
			const float4		 aj						  = sesContext.getAtom( circle.j );
			const CircleGeometry geometry				  = sesContext.getCircleGeometry( ai, aj );
			const uint32_t		 startCircleIntersections = sesContext.circlesIntersectionStartId[ trimmedId ];

			memcpy(
				intersectionIds,
				sesContext.circlesIntersectionIds + startCircleIntersections,
				sizeof( uint32_t ) * circle.intersectionNb
			);

			// Find a first intersection to start the rotation
			const uint32_t startIntersectionId = intersectionIds[ 0 ];

			// If the current circle was the one placed at the i -> k for the first intersection, its direction is
			// reversed
			const int4	 startingAtomIds  = intersectionAtomIds[ startIntersectionId ];
			const float4 startingPosition = intersectionPositions[ startIntersectionId ];
			const bool	 isSecondary	  = circle.i == startingAtomIds.x && circle.j == startingAtomIds.z;
			pickedDirection				  = sign( startingPosition.w ) * ( isSecondary ? -1.f : 1.f );

			// Initialize first intersection as the start of the rotation
			circleAngles[ 0 ]		   = startAngle;
			circleIntersectionIds[ 0 ] = 0;

			const float3 startingIntersectionPos = make_float3( startingPosition );
			const float3 ref					 = startingIntersectionPos - geometry.center;
			for ( uint8_t i = 1; i < circle.intersectionNb; i++ )
			{
				const uint32_t currentIntersectionId = intersectionIds[ i ];
				const float3   currentPoint			 = make_float3( intersectionPositions[ currentIntersectionId ] );

				float3 v1 = ref;
				float3 v2 = normalize( currentPoint - geometry.center );
				if ( pickedDirection < 0.f )
					thrust::swap( v1, v2 );

				const float angle = angleBetweenEdges( v1, v2, geometry.normal );

				// Keeping thread data coherent since the sorting will happens at warp / group scope
				circleAngles[ i ]		   = startAngle + angle;
				circleIntersectionIds[ i ] = i;
			}
		}

		// Performs sorting at warp level
		auto warp = cg::tiled_partition<WarpThreadNb>( cg::this_thread_block() );
		warp.sync();
		const uint32_t warpId = warp.meta_group_rank();
		WarpMergeSortT( tempStorage[ warpId ] ).Sort( circleAngles, circleIntersectionIds, LessOperator() );

		if ( circleWithSegmentId < circleWithSegmentNb )
		{
			// Segments can now be created by just applying couples in angular order
			const uint32_t segmentsStartIdx		= segmentsIndices[ trimmedId ];
			const uint8_t  intersectionCoupleNb = circle.intersectionNb / 2u;
			for ( uint8_t k = 0; k < intersectionCoupleNb; k++ )
			{
				const uint32_t firstId	= circleIntersectionIds[ 2 * k + 0 ];
				const uint32_t secondId = circleIntersectionIds[ 2 * k + 1 ];

				uint32_t firstInt  = intersectionIds[ firstId ];
				uint32_t secondInt = intersectionIds[ secondId ];
				if ( pickedDirection < 0.f )
					thrust::swap( firstInt, secondInt );

				segmentsData[ segmentsStartIdx + k ] = make_uint4( circle.i, circle.j, firstInt, secondInt );
			}
		}
	}
} // namespace bcs::sesdf
