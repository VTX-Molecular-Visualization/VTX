#include <device_launch_parameters.h>
#include <thrust/async/sort.h>
#include <thrust/sort.h>
#include <vector_functions.h>

#include "bcs/cuda/grid.cuh"
#include "bcs/cuda/math.cuh"
#include "bcs/cuda/setup.cuh"

namespace bcs
{
    // Based on NVIDIA particle simulation samples
    // Ref:
    // https://github.com/NVIDIA/cuda-samples/blob/master/Samples/2_Concepts_and_Techniques/particles/particles_kernel_impl.cuh
    __global__ void computeHashes( const GridInfo       gridInfo,
                                   const uint32_t       elementNb,
                                   const float4 * const positions,
                                   uint32_t *           hashes,
                                   uint32_t *           indices )
    {
        const uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
        if ( index >= elementNb )
            return;

        const float4 p = positions[ index ];

        // get address in grid
        const uint32_t hash = gridInfo.getHash( make_float3( p ) );

        // store grid hash and particle index
        hashes[ index ]  = hash;
        indices[ index ] = index;
    }

    __global__ void buildAccelerationGrid( const uint32_t         elementNb,
                                           const uint32_t * const hashes,
                                           const uint32_t * const indices,
                                           const float4 * const   oldPos,
                                           uint32_t *             cellStarts,
                                           uint32_t *             cellEnds,
                                           float4 *               sortedPos )
    {
        extern __shared__ uint32_t sharedHash[]; // blockSize + 1 elements
        const uint32_t             index = blockIdx.x * blockDim.x + threadIdx.x;

        uint32_t hash;
        // handle case when no. of particles not multiple of block size
        if ( index < elementNb )
        {
            hash = hashes[ index ];

            // Load hash data into shared memory so that we can look
            // at neighboring particle's hash value without loading
            // two hash values per thread
            sharedHash[ threadIdx.x + 1 ] = hash;

            if ( index > 0 && threadIdx.x == 0 )
            {
                // first thread in block must load neighbor particle hash
                sharedHash[ 0 ] = hashes[ index - 1 ];
            }
        }

        __syncthreads();

        if ( index < elementNb )
        {
            // If this particle has a different cell index to the previous
            // particle then it must be the first particle in the cell,
            // so store the index of this particle in the cell.
            // As it isn't the first particle, it must also be the cell end of
            // the previous particle's cell

            const uint32_t lastHash = sharedHash[ threadIdx.x ];
            if ( index == 0 || hash != lastHash )
            {
                cellStarts[ hash ] = index;
                if ( index > 0 )
                    cellEnds[ lastHash ] = index;
            }

            if ( index == elementNb - 1 )
                cellEnds[ hash ] = index + 1;

            // Now use the sorted index to reorder the pos data
            const uint32_t sortedIndex = indices[ index ];
            sortedPos[ index ]         = oldPos[ sortedIndex ];
        }
    }

    AccelerationGrid::AccelerationGrid( GridInfo configuration, cudaStream_t stream ) :
        m_configuration( configuration ),
        m_cellNb( configuration.size.x * configuration.size.y * configuration.size.z ),
        m_dCellsStart( DeviceBuffer::Typed<uint32_t>( m_cellNb, stream ) ),
        m_dCellsEnd( DeviceBuffer::Typed<uint32_t>( m_cellNb, stream ) )
    {
    }

    void AccelerationGrid::build( const uint32_t elementNb, const float4 * const positions )
    {
        cudaMemset( m_dCellsStart.get<uint32_t>(), EmptyGridCellValue, sizeof( uint32_t ) * m_cellNb );

        // Re-allocatation only if needed
        if ( !m_dHashes || m_dHashes.size<uint32_t>() < elementNb )
            m_dHashes = DeviceBuffer::Typed<uint32_t>( elementNb );

        if ( !m_dIndices || m_dIndices.size<uint32_t>() < elementNb )
            m_dIndices = DeviceBuffer::Typed<uint32_t>( elementNb );

        if ( !m_dSortedPositions || m_dSortedPositions.size<float4>() < elementNb )
            m_dSortedPositions = DeviceBuffer::Typed<float4>( elementNb );

        auto [ numBlocks, numThreads ] = KernelConfig::From( elementNb, 256 );

        uint32_t * hashes  = m_dHashes.get<uint32_t>();
        uint32_t * indices = m_dIndices.get<uint32_t>();

        computeHashes<<<numBlocks, numThreads>>>( m_configuration, elementNb, positions, hashes, indices );
        cudaCheck( "Hashes computation failed" );

        thrust::sort_by_key( thrust::device, hashes, hashes + elementNb, indices );

        const uint32_t sharedMemorySize = sizeof( uint32_t ) * ( numThreads.x + 1 );
        buildAccelerationGrid<<<numBlocks, numThreads, sharedMemorySize>>>( elementNb,
                                                                            hashes,
                                                                            indices,
                                                                            positions,
                                                                            m_dCellsStart.get<uint32_t>(),
                                                                            m_dCellsEnd.get<uint32_t>(),
                                                                            m_dSortedPositions.get<float4>() );
        cudaCheck( "Acceleration grid construction failed" );
    }

} // namespace bcs
