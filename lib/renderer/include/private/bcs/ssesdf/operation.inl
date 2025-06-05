#pragma executioncharacterset( "utf-8" )

#include "bcs/cuda/execution.cuh"
#include "bcs/ssesdf/graphics.hpp"
#include "bcs/ssesdf/operations.cuh"
#include <cooperative_groups.h>
#include <thrust/count.h>
#include <thrust/sort.h>

namespace bcs::ssesdf
{
	namespace cg = cooperative_groups;

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
	struct IsComplete : thrust::unary_function<uint8_t, uint8_t>
	{
		__host__ __device__ uint8_t operator()( uint8_t x ) { return uint8_t( x == 2 ); }
	};

	template<uint32_t MaxNeighborPerAtom>
	std::pair<ResultBuffer, ResultBuffer> findIntersections( SsesdfContext & sesContext, bool graphics )
	{
		ResultBuffer intersectionBuffer;
		{
			auto tempIntersectionsIds
				= DeviceBuffer::Typed<int4>( sesContext.intersectedCircleNb * sesContext.maxIntersectionsPerCircles );

			findIntersectionsBetweenCircles<MaxNeighborPerAtom>
				<<<sesContext.atomNb, sesContext.maxNeighborPerAtom>>>( sesContext, tempIntersectionsIds.get<int4>() );
			cudaCheck( "Intersection construction failed" );

			mmemcpy<MemcpyType::DeviceToHost>( sesContext.hIntersectionNb, sesContext.dIntersectionNb, 1 );

			if ( *sesContext.hIntersectionNb > sesContext.intersectedCircleNb * sesContext.maxIntersectionsPerCircles )
			{
				fmt::print(
					"Error: More intersection than expected",
					*sesContext.hIntersectionNb,
					sesContext.intersectedCircleNb * sesContext.maxIntersectionsPerCircles
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
			copy(
				intersectionBuffer.get<int4>() + *sesContext.hIntersectionNb,
				tempIntersectionsIds.get<int4>(),
				*sesContext.hIntersectionNb
			);
		}

		{
			auto [ blockNb, threadNb ] = KernelConfig::From( *sesContext.hIntersectionNb, 256 );
			fillIntersections<<<blockNb, threadNb>>>(
				sesContext,
				intersectionBuffer.get<int4>() + *sesContext.hIntersectionNb,
				intersectionBuffer.get<float4>()
			);
			cudaCheck( "Intersection fill failed" );
		}

		uint32_t toroidalPatches = thrust::count_if(
			thrust::device,
			sesContext.visibilityStatus,
			sesContext.visibilityStatus + sesContext.atomNb * sesContext.maxNeighborPerAtom,
			IsComplete()
		);

		auto patchesIds = DeviceBuffer::Typed<uint32_t>( toroidalPatches );
		thrust::copy_if(
			thrust::device,
			thrust::make_counting_iterator<uint32_t>( 0 ),
			thrust::make_counting_iterator<uint32_t>( sesContext.getMaximumCircleNb() ),
			sesContext.visibilityStatus,
			patchesIds.get<uint32_t>(),
			IsComplete {}
		);

		ResultBuffer circles   = ResultBuffer( ( sizeof( float4 ) * 3 ) * ( toroidalPatches ), false, graphics );
		float4 *	 positions = circles.get<float4>();
		float4 *	 axis	   = circles.get<float4>() + toroidalPatches;
		float4 *	 visibilitySpheres = circles.get<float4>() + toroidalPatches * 2ull;

		auto [ blockNb, threadNb ] = KernelConfig::From( toroidalPatches, 256 );
		fillCircles<<<blockNb, threadNb>>>(
			sesContext, toroidalPatches, patchesIds.get<uint32_t>(), positions, axis, visibilitySpheres
		);
		cudaCheck( "Circles fill failed" );

		return std::make_pair( std::move( circles ), std::move( intersectionBuffer ) );
	}

	template<uint32_t MaxNeighborPerAtom>
	__global__ void findIntersectionsBetweenCircles( SsesdfContext sesContext, int4 * intersectionAtomIds )
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
                            const int3   ids  = make_int3( blockIdx.x * sesContext.maxNeighborPerAtom + localNeighbor,
                                                        blockIdx.x * sesContext.maxNeighborPerAtom + iterationLocalK,
                                                        ij.j * sesContext.maxNeighborPerAtom + iterationKjIndex );
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
			const int4 current							   = groupIntersectionsIds[ i ];
			intersectionAtomIds[ startIntersectionId + i ] = current;
		}
	}
} // namespace bcs::ssesdf
