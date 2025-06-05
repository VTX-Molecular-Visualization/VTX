#include <algorithm>

#include <cuda_runtime.h>
#include <glm/common.hpp>
#include <helper_cuda.h>

#include "bcs/core/molecule.hpp"
#include "bcs/cuda/grid.cuh"
#include "bcs/ssesdf/data.cuh"
#include "bcs/ssesdf/operations.cuh"
#include "bcs/ssesdf/ssesdf.hpp"

namespace bcs
{
    constexpr uint16_t Ssesdf::MaxNeighborPerAtom         = BCS_SSESDF_MAXNEIGHBORPERATOM;
    constexpr uint16_t Ssesdf::MaxIntersectionsPerCircles = 2;
    constexpr uint16_t Ssesdf::MaxIntersectionNeighbors   = 32;

    Ssesdf::Ssesdf( ConstSpan<Vec4f> molecule, const Aabb & aabb, const float probeRadius, bool buildSurface, bool graphics ) :
        m_molecule( molecule ), m_probeRadius( probeRadius ), m_atomNb( molecule.size ), m_graphics(graphics)
    {
        constexpr float maxVdwRadius = 3.48f;

        const glm::vec3 worldOrigin = aabb.min - maxVdwRadius - m_probeRadius;
        const glm::vec3 worldSize   = glm::abs( aabb.max + maxVdwRadius + m_probeRadius - worldOrigin );

        const uint32_t  gridSize = static_cast<uint32_t>( nextPowerOfTwoValue( nextPowerOfTwoExponent( m_atomNb ) ) );
        const glm::vec3 cellSize = worldSize / static_cast<float>( gridSize );

        m_gridConfiguration.worldOrigin = make_float3( worldOrigin.x, worldOrigin.y, worldOrigin.z );
        m_gridConfiguration.cellSize    = make_float3( cellSize.x, cellSize.y, cellSize.z );
        m_gridConfiguration.size        = make_int3( static_cast<int>( gridSize ) );

        // // Pick the device with highest Gflops/s
        cudaDeviceProp deviceProp;
        int            deviceId = gpuGetMaxGflopsDeviceId();
        cudaCheck( cudaSetDevice( deviceId ) );
        cudaCheck( cudaGetDeviceProperties( &deviceProp, deviceId ) );

        int isMemPoolSupported = 0;
        cudaCheck( cudaDeviceGetAttribute( &isMemPoolSupported, cudaDevAttrMemoryPoolsSupported, deviceId ) );
        assert( isMemPoolSupported );

        cudaCheck( cudaSetDevice( deviceId ) );

        cudaCheck( cudaMallocHost( &m_hIntersectionNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMallocHost( &m_hVisibleCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMalloc( &m_dIntersectionNb, sizeof( uint32_t ) ) );

        m_dAtoms = ResultBuffer::Typed<float4>( m_atomNb, false, m_graphics );

        m_dAtomIndices            = DeviceBuffer::Typed<uint32_t>( m_atomNb );
        m_dAtomNeighborsCount     = DeviceBuffer::Typed<uint32_t>( m_atomNb );
        m_dAtomNeighborsIndices   = DeviceBuffer::Typed<uint32_t>( MaxNeighborPerAtom * m_atomNb );
        m_dCircleVisibilityStatus = DeviceBuffer::Typed<uint8_t>( MaxNeighborPerAtom * m_atomNb + 1 );

        cudaMemPool_t memPool;
        cudaCheck( cudaDeviceGetDefaultMemPool( &memPool, deviceId ) );

        constexpr uint64_t thresholdVal = std::numeric_limits<uint64_t>::max();
        cudaCheck( cudaMemPoolSetAttribute( memPool, cudaMemPoolAttrReleaseThreshold, (void *)&thresholdVal ) );
        m_accelerationGrid = AccelerationGrid( m_gridConfiguration );

        if ( buildSurface )
            build();
    }

    Ssesdf::Ssesdf( Ssesdf && other )
    {
        std::swap( m_molecule, other.m_molecule );
        std::swap( m_probeRadius, other.m_probeRadius );
        std::swap( m_atomNb, other.m_atomNb );

        std::swap( m_intersectionNb, other.m_intersectionNb );
        std::swap( m_intersectionWithNeighborNb, other.m_intersectionWithNeighborNb );
        std::swap( m_circleNb, other.m_circleNb );

        std::swap( m_gridConfiguration, other.m_gridConfiguration );

        // Preallocated buffers
        std::swap( m_dAtomIndices, other.m_dAtomIndices );
        std::swap( m_dAtomNeighborsCount, other.m_dAtomNeighborsCount );
        std::swap( m_dAtomNeighborsIndices, other.m_dAtomNeighborsIndices );
        std::swap( m_dCircleVisibilityStatus, other.m_dCircleVisibilityStatus );
        std::swap( m_accelerationGrid, other.m_accelerationGrid );

        std::swap( m_dIntersectionNb, other.m_dIntersectionNb );
        std::swap( m_hIntersectionNb, other.m_hIntersectionNb );
        std::swap( m_hVisibleCircleNb, other.m_hVisibleCircleNb );

        // Rendering buffers
        std::swap( m_dAtoms, other.m_dAtoms );
        std::swap( m_dIntersections, other.m_dIntersections );
    }

    Ssesdf & Ssesdf::operator=( Ssesdf && other )
    {
        std::swap( m_molecule, other.m_molecule );
        std::swap( m_probeRadius, other.m_probeRadius );
        std::swap( m_atomNb, other.m_atomNb );

        std::swap( m_intersectionNb, other.m_intersectionNb );
        std::swap( m_intersectionWithNeighborNb, other.m_intersectionWithNeighborNb );
        std::swap( m_circleNb, other.m_circleNb );

        std::swap( m_gridConfiguration, other.m_gridConfiguration );

        // Preallocated buffers
        std::swap( m_dAtomIndices, other.m_dAtomIndices );
        std::swap( m_dAtomNeighborsCount, other.m_dAtomNeighborsCount );
        std::swap( m_dAtomNeighborsIndices, other.m_dAtomNeighborsIndices );
        std::swap( m_dCircleVisibilityStatus, other.m_dCircleVisibilityStatus );
        std::swap( m_accelerationGrid, other.m_accelerationGrid );

        std::swap( m_dIntersectionNb, other.m_dIntersectionNb );
        std::swap( m_hIntersectionNb, other.m_hIntersectionNb );
        std::swap( m_hVisibleCircleNb, other.m_hVisibleCircleNb );

        // Rendering buffers
        std::swap( m_dAtoms, other.m_dAtoms );
        std::swap( m_dIntersections, other.m_dIntersections );

        return *this;
    }

    Ssesdf::~Ssesdf()
    {
        cudaCheck( cudaFreeHost( m_hIntersectionNb ) );
        cudaCheck( cudaFreeHost( m_hVisibleCircleNb ) );
        cudaCheck( cudaFree( m_dIntersectionNb ) );
    }

    void Ssesdf::build()
    {
        {
            ssesdf::SsesdfContext sesContext {};
            sesContext.probeRadius                = m_probeRadius;
            sesContext.atomNb                     = m_atomNb;
            sesContext.atoms                      = m_dAtoms.get<float4>();
            sesContext.sortedToInitialIndices     = m_dAtomIndices.get<uint32_t>();
            sesContext.neighborNb                 = m_dAtomNeighborsCount.get<uint32_t>();
            sesContext.neighborIds                = m_dAtomNeighborsIndices.get<uint32_t>();
            sesContext.visibilityStatus           = m_dCircleVisibilityStatus.get<uint8_t>();
            sesContext.maxNeighborPerAtom         = MaxNeighborPerAtom;
            sesContext.maxIntersectionsPerCircles = MaxIntersectionsPerCircles;
            sesContext.maxIntersectionNeighbors   = MaxIntersectionNeighbors;

            sesContext.dIntersectionNb = m_dIntersectionNb;
            cudaMemset( sesContext.dIntersectionNb, 0, sizeof( uint32_t ) );
            sesContext.hIntersectionNb  = m_hIntersectionNb;
            *sesContext.hIntersectionNb = 0;

            sesContext.hVisibleCircleNb  = m_hVisibleCircleNb;
            *sesContext.hVisibleCircleNb = 0;

            // #1: CPU => GPU
            mmemcpy<MemcpyType::HostToDevice>(
                m_dAtoms.get<float4>(), reinterpret_cast<const float4 *>( m_molecule.ptr ), m_atomNb );

            // #2: Find Circles
            ssesdf::findCircles( m_accelerationGrid, sesContext );
            m_circleNb = *sesContext.hVisibleCircleNb;

            if ( m_circleNb > 0 )
            {
                // #3: Find intersections
                auto [ dCircles, dIntersections ] = ssesdf::findIntersections<MaxNeighborPerAtom>( sesContext, m_graphics );
                m_dCircles                        = std::move( dCircles );
                m_circleNb                        = m_dCircles.size() / ( sizeof( float4 ) * 3 );

                m_intersectionNb             = *sesContext.hIntersectionNb;
                m_intersectionWithNeighborNb = sesContext.intersectionWithNeighborNb;
                m_dIntersections             = std::move( dIntersections );

                // #4: Build P_- neighbors
                ssesdf::handleIntersectionSingularities( m_accelerationGrid,
                                                         sesContext,
                                                         m_dIntersections.get<float4>(),
                                                         m_dIntersections.get<int4>() + m_intersectionNb,
                                                         m_dIntersections.get<float4>() + m_intersectionNb * 2 );

                m_dAtoms.unmap();
                m_dCircles.unmap();
                m_dIntersections.unmap();
            }
        }
    }

    ssesdf::SsesdfGraphics Ssesdf::getGraphics() const
    {
        return { m_atomNb,
                 { m_dAtoms.getId(), 0, m_atomNb * sizeof( float4 ) },

                 // Toroidal patches
                 m_circleNb,
                 { m_dCircles.getId(), 0, m_circleNb * sizeof( float4 ) },                             // circlePosition
                 { m_dCircles.getId(), m_circleNb * sizeof( float4 ), m_circleNb * sizeof( float4 ) }, // circleAxis
                 { m_dCircles.getId(), m_circleNb * sizeof( float4 ) * 2, m_circleNb * sizeof( float4 ) }, // circleVs

                 // Concave patches
                 m_intersectionNb,
                 { m_dIntersections.getId(), 0, m_intersectionNb * sizeof( float4 ) },
                 { m_dIntersections.getId(), m_intersectionNb * sizeof( int4 ), m_intersectionNb * sizeof( int4 ) },
                 { m_dIntersections.getId(),
                   m_intersectionNb * ( sizeof( int4 ) + sizeof( float4 ) ),
                   m_intersectionWithNeighborNb * MaxIntersectionNeighbors * sizeof( float4 ) },

                 m_probeRadius,
                 MaxIntersectionNeighbors };
    }

    ssesdf::SsesdfData Ssesdf::getData()
    {
        float4 * dAtoms = m_dAtoms.get<float4>();
        return { m_atomNb,
                 dAtoms,

                 // Toroidal patches
                 m_circleNb,
                 m_dCircles.get<float4>(),
                 m_dCircles.get<float4>( m_circleNb * sizeof( float4 ) ),
                 m_dCircles.get<float4>( m_circleNb * sizeof( float4 ) * 2 ),

                 // Concave patches
                 m_intersectionNb,
                 m_dIntersections.get<float4>(),
                 m_dIntersections.get<int4>( m_intersectionNb * sizeof( float4 ) ),
                 m_dIntersections.get<float4>( m_intersectionNb * sizeof( float4 ) * 2 ),

                 m_probeRadius,
                 MaxIntersectionNeighbors };
    }

    uint16_t Ssesdf::getMaxNeighborPerAtom() const { return MaxNeighborPerAtom; }
    uint16_t Ssesdf::getMaxIntersectionsPerCircles() const { return MaxIntersectionsPerCircles; }
    uint16_t Ssesdf::getMaxIntersectionNeighbors() const { return MaxIntersectionNeighbors; }

    std::vector<uint32_t> Ssesdf::getAtomIndices() { return m_dAtomIndices.toHost<uint32_t>(); }

    DeviceBuffer & Ssesdf::getDAtomIndices() { return m_dAtomIndices; }
    DeviceBuffer & Ssesdf::getDAtomNeighborsCount() { return m_dAtomNeighborsCount; }
    DeviceBuffer & Ssesdf::getDAtomNeighborsIndices() { return m_dAtomNeighborsIndices; }
    DeviceBuffer & Ssesdf::getDCircleVisibilityStatus() { return m_dCircleVisibilityStatus; }

} // namespace bcs
