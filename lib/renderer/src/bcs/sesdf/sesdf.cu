#include <algorithm>

#include <cuda_runtime.h>
#include <glm/common.hpp>
#include <cuda_helper/helper_cuda.h>

//#include "bcs/core/molecule.hpp"
#include "bcs/cuda/grid.cuh"
#include "bcs/sesdf/data.cuh"
#include "bcs/sesdf/operations.cuh"
#include "bcs/sesdf/sesdf.hpp"

namespace bcs
{
    constexpr uint16_t Sesdf::MaxNeighborPerAtom             = BCS_SESDF_MAXNEIGHBORPERATOM;
    constexpr uint16_t Sesdf::MaxMeanIntersectionsPerCircles = 2;
    constexpr uint16_t Sesdf::MaxIntersectionsPerCircles     = 16;
    constexpr uint16_t Sesdf::MaxIntersectionNeighbors       = 32;

    Sesdf::Sesdf( ConstSpan<Vec4f> molecule, const Aabb & aabb, const float probeRadius, bool buildSurface, bool graphics ) :
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

        cudaCheck( cudaMallocHost( &m_hIntersectedCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMallocHost( &m_hIntersectionNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMallocHost( &m_hFullCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMallocHost( &m_hSegmentCount, sizeof( uint32_t ) ) );
        cudaCheck( cudaMallocHost( &m_hVisibleCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMalloc( &m_dIntersectedCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMalloc( &m_dIntersectionNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMalloc( &m_dFullCircleNb, sizeof( uint32_t ) ) );
        cudaCheck( cudaMalloc( &m_dSegmentCount, sizeof( uint32_t ) ) );

        m_dAtoms         = ResultBuffer::Typed<float4>( m_atomNb, false, m_graphics);
        m_dConvexPatches = ResultBuffer::Typed<uint2>( m_atomNb, false, m_graphics );

        m_dAtomIndices            = DeviceBuffer::Typed<uint32_t>( m_atomNb );
        m_dAtomNeighborsCount     = DeviceBuffer::Typed<uint32_t>( m_atomNb );
        m_dAtomNeighborsIndices   = DeviceBuffer::Typed<uint32_t>( MaxNeighborPerAtom * m_atomNb );
        m_dCircleVisibilityStatus = DeviceBuffer::Typed<uint8_t>( MaxNeighborPerAtom * m_atomNb + 1 );
        m_dGlobalToTrimmedId      = DeviceBuffer::Typed<uint32_t>( MaxNeighborPerAtom * m_atomNb + 1 );

        cudaMemPool_t memPool;
        cudaCheck( cudaDeviceGetDefaultMemPool( &memPool, deviceId ) );

        constexpr uint64_t thresholdVal = std::numeric_limits<uint64_t>::max();
        cudaCheck( cudaMemPoolSetAttribute( memPool, cudaMemPoolAttrReleaseThreshold, (void *)&thresholdVal ) );
        m_accelerationGrid = AccelerationGrid( m_gridConfiguration );

        if ( buildSurface )
            build();
    }

    Sesdf::Sesdf( Sesdf && other )
    {
        std::swap( m_molecule, other.m_molecule );
        std::swap( m_probeRadius, other.m_probeRadius );
        std::swap( m_atomNb, other.m_atomNb );

        std::swap( m_convexPatchNb, other.m_convexPatchNb );
        std::swap( m_segmentNb, other.m_segmentNb );
        std::swap( m_intersectionNb, other.m_intersectionNb );
        std::swap( m_fullCircleNb, other.m_fullCircleNb );
        std::swap( m_sectorNb, other.m_sectorNb );

        std::swap( m_gridConfiguration, other.m_gridConfiguration );

        // Preallocated buffers
        std::swap( m_dAtomIndices, other.m_dAtomIndices );
        std::swap( m_dAtomNeighborsCount, other.m_dAtomNeighborsCount );
        std::swap( m_dAtomNeighborsIndices, other.m_dAtomNeighborsIndices );
        std::swap( m_dCircleVisibilityStatus, other.m_dCircleVisibilityStatus );
        std::swap( m_dGlobalToTrimmedId, other.m_dGlobalToTrimmedId );
        std::swap( m_accelerationGrid, other.m_accelerationGrid );

        std::swap( m_dIntersectedCircleNb, other.m_dIntersectedCircleNb );
        std::swap( m_hIntersectedCircleNb, other.m_hIntersectedCircleNb );
        std::swap( m_dIntersectionNb, other.m_dIntersectionNb );
        std::swap( m_hIntersectionNb, other.m_hIntersectionNb );
        std::swap( m_dFullCircleNb, other.m_dFullCircleNb );
        std::swap( m_hFullCircleNb, other.m_hFullCircleNb );
        std::swap( m_dSegmentCount, other.m_dSegmentCount );
        std::swap( m_hSegmentCount, other.m_hSegmentCount );
        std::swap( m_hVisibleCircleNb, other.m_hVisibleCircleNb );

        // Rendering buffers
        std::swap( m_dAtoms, other.m_dAtoms );
        std::swap( m_dIntersections, other.m_dIntersections );
        std::swap( m_dSegments, other.m_dSegments );
        std::swap( m_dConvexPatches, other.m_dConvexPatches );
        std::swap( m_dFCircleAndSectors, other.m_dFCircleAndSectors );
    }

    Sesdf & Sesdf::operator=( Sesdf && other )
    {
        std::swap( m_molecule, other.m_molecule );
        std::swap( m_probeRadius, other.m_probeRadius );
        std::swap( m_atomNb, other.m_atomNb );

        std::swap( m_convexPatchNb, other.m_convexPatchNb );
        std::swap( m_segmentNb, other.m_segmentNb );
        std::swap( m_intersectionNb, other.m_intersectionNb );
        std::swap( m_fullCircleNb, other.m_fullCircleNb );
        std::swap( m_sectorNb, other.m_sectorNb );

        std::swap( m_gridConfiguration, other.m_gridConfiguration );

        // Preallocated buffers
        std::swap( m_dAtomIndices, other.m_dAtomIndices );
        std::swap( m_dAtomNeighborsCount, other.m_dAtomNeighborsCount );
        std::swap( m_dAtomNeighborsIndices, other.m_dAtomNeighborsIndices );
        std::swap( m_dCircleVisibilityStatus, other.m_dCircleVisibilityStatus );
        std::swap( m_dGlobalToTrimmedId, other.m_dGlobalToTrimmedId );
        std::swap( m_accelerationGrid, other.m_accelerationGrid );

        std::swap( m_dIntersectedCircleNb, other.m_dIntersectedCircleNb );
        std::swap( m_hIntersectedCircleNb, other.m_hIntersectedCircleNb );
        std::swap( m_dIntersectionNb, other.m_dIntersectionNb );
        std::swap( m_hIntersectionNb, other.m_hIntersectionNb );
        std::swap( m_dFullCircleNb, other.m_dFullCircleNb );
        std::swap( m_hFullCircleNb, other.m_hFullCircleNb );
        std::swap( m_dSegmentCount, other.m_dSegmentCount );
        std::swap( m_hSegmentCount, other.m_hSegmentCount );
        std::swap( m_hVisibleCircleNb, other.m_hVisibleCircleNb );

        // Rendering buffers
        std::swap( m_dAtoms, other.m_dAtoms );
        std::swap( m_dIntersections, other.m_dIntersections );
        std::swap( m_dSegments, other.m_dSegments );
        std::swap( m_dConvexPatches, other.m_dConvexPatches );
        std::swap( m_dFCircleAndSectors, other.m_dFCircleAndSectors );

        return *this;
    }

    Sesdf::~Sesdf()
    {
        cudaCheck( cudaFreeHost( m_hIntersectedCircleNb ) );
        cudaCheck( cudaFreeHost( m_hIntersectionNb ) );
        cudaCheck( cudaFreeHost( m_hFullCircleNb ) );
        cudaCheck( cudaFreeHost( m_hSegmentCount ) );
        cudaCheck( cudaFreeHost( m_hVisibleCircleNb ) );
        cudaCheck( cudaFree( m_dIntersectedCircleNb ) );
        cudaCheck( cudaFree( m_dIntersectionNb ) );
        cudaCheck( cudaFree( m_dFullCircleNb ) );
        cudaCheck( cudaFree( m_dSegmentCount ) );
    }

    void Sesdf::build()
    {
        {
            sesdf::SesdfContext sesContext {};
            sesContext.probeRadius                = m_probeRadius;
            sesContext.atomNb                     = m_atomNb;
            sesContext.atoms                      = m_dAtoms.get<float4>();
            sesContext.sortedToInitialIndices     = m_dAtomIndices.get<uint32_t>();
            sesContext.neighborNb                 = m_dAtomNeighborsCount.get<uint32_t>();
            sesContext.neighborIds                = m_dAtomNeighborsIndices.get<uint32_t>();
            sesContext.visibilityStatus           = m_dCircleVisibilityStatus.get<uint8_t>();
            sesContext.maxNeighborPerAtom         = MaxNeighborPerAtom;
            sesContext.maxIntersectionsPerCircles = MaxMeanIntersectionsPerCircles;
            sesContext.maxIntersectionNeighbors   = MaxIntersectionNeighbors;
            sesContext.globalToTrimmedId          = m_dGlobalToTrimmedId.get<uint32_t>();

            sesContext.dIntersectedCircleNb = m_dIntersectedCircleNb;
            cudaMemset( sesContext.dIntersectedCircleNb, 0, sizeof( uint32_t ) );
            sesContext.hIntersectedCircleNb  = m_hIntersectedCircleNb;
            *sesContext.hIntersectedCircleNb = 0;

            sesContext.dIntersectionNb = m_dIntersectionNb;
            cudaMemset( sesContext.dIntersectionNb, 0, sizeof( uint32_t ) );
            sesContext.hIntersectionNb  = m_hIntersectionNb;
            *sesContext.hIntersectionNb = 0;

            sesContext.dFullCircleNb = m_dFullCircleNb;
            cudaMemset( sesContext.dFullCircleNb, 0, sizeof( uint32_t ) );
            sesContext.hFullCircleNb  = m_hFullCircleNb;
            *sesContext.hFullCircleNb = 0;

            sesContext.dSegmentCount = m_dSegmentCount;
            cudaMemset( sesContext.dSegmentCount, 0, sizeof( uint32_t ) );
            sesContext.hSegmentCount  = m_hSegmentCount;
            *sesContext.hSegmentCount = 0;

            sesContext.hVisibleCircleNb  = m_hVisibleCircleNb;
            *sesContext.hVisibleCircleNb = 0;

            // #1: CPU => GPU
            mmemcpy<MemcpyType::HostToDevice>(
                m_dAtoms.get<float4>(), reinterpret_cast<const float4 *>( m_molecule.ptr ), m_atomNb );

            // #2: Find Circles
            auto [ fCircleAndSectors, trimmedToGlobalId ] = sesdf::findCircles<MaxNeighborPerAtom>(
                m_accelerationGrid, sesContext, m_dConvexPatches.get<uint2>(), m_graphics );

            sesContext.trimmedToGlobalId = trimmedToGlobalId.get<uint32_t>();
            m_dFCircleAndSectors         = std::move( fCircleAndSectors );
            m_convexPatchNb              = m_atomNb;
            m_fullCircleNb               = *sesContext.hFullCircleNb;
            m_sectorNb = ( m_dFCircleAndSectors.size() - m_fullCircleNb * sizeof( uint2 ) ) / sizeof( uint32_t );

            if ( *sesContext.hIntersectedCircleNb > 0 )
            {
                // #3: Find intersections
                auto circlesIntersectionsNb = DeviceBuffer::Typed<uint32_t>( *sesContext.hIntersectedCircleNb, true );
                sesContext.circlesIntersectionNb = circlesIntersectionsNb.get<uint32_t>();

                auto [ dIntersections, startandIdList ] = sesdf::findIntersections<MaxNeighborPerAtom>( sesContext, m_graphics );

                m_intersectionNb             = *sesContext.hIntersectionNb;
                m_intersectionWithNeighborNb = sesContext.intersectionWithNeighborNb;
                m_dIntersections             = std::move( dIntersections );

                sesContext.circlesIntersectionStartId = startandIdList.get<uint32_t>();
                sesContext.circlesIntersectionIds = startandIdList.get<uint32_t>() + *sesContext.hIntersectedCircleNb;

                // #4: Build P_t
                auto segmentsData = sesdf::buildConicPatches<MaxIntersectionsPerCircles>(
                    sesContext,
                    m_dIntersections.get<const float4>( m_intersectionNb * sizeof( int4 ) ),
                    m_dIntersections.get<const int4>(),
                    m_graphics );

                m_dSegments = std::move( segmentsData );
                m_segmentNb = *sesContext.hSegmentCount;

                // #5: Build P_- neighbors
                sesdf::handleIntersectionSingularities(
                    m_accelerationGrid,
                    sesContext,
                    m_dIntersections.get<const float4>( m_intersectionNb * sizeof( int4 ) ),
                    m_dIntersections.get<int4>(),
                    m_dIntersections.get<float4>( m_intersectionNb * ( sizeof( int4 ) + sizeof( float4 ) ) ) );

                m_dIntersections.unmap();
                m_dSegments.unmap();
                m_dConvexPatches.unmap();
                m_dFCircleAndSectors.unmap();
            }
        }
    }

    sesdf::SesdfGraphics Sesdf::getGraphics() const
    {
        assert(m_graphics && "Computation has not been configured for graphics!");

        return { m_atomNb,
                 { m_dAtoms.getId(), m_atomNb * sizeof( float4 ) },

                 // Convex patch data
                 m_convexPatchNb,
                 { m_dConvexPatches.getId(), m_atomNb * sizeof( uint2 ) },

                 m_sectorNb,
                 { m_dFCircleAndSectors.getId(),
                   m_fullCircleNb * sizeof( uint2 ) + ( ( m_fullCircleNb * sizeof( uint2 ) ) % sizeof( float4 ) ),
                   m_sectorNb * sizeof( float4 ) },

                 // Toroidal patches
                 m_segmentNb,
                 { m_dSegments.getId(), m_segmentNb * sizeof( uint4 ) },

                 m_fullCircleNb,
                 { m_dFCircleAndSectors.getId(), m_fullCircleNb * sizeof( uint2 ) },

                 // Concave patches
                 m_intersectionNb,
                 { m_dIntersections.getId(), m_intersectionNb * sizeof( float4 ), m_intersectionNb * sizeof( int4 ) },
                 { m_dIntersections.getId(), m_intersectionNb * sizeof( int4 ) },
                 { m_dIntersections.getId(),
                   m_intersectionNb * ( sizeof( int4 ) + sizeof( float4 ) ),
                   m_intersectionWithNeighborNb * MaxIntersectionNeighbors * sizeof( float4 ) },

                 m_probeRadius,
                 MaxIntersectionNeighbors };
    }


    sesdf::SesdfData Sesdf::getData()
    {
        float4 * dAtoms         = m_dAtoms.get<float4>();
        uint2 *  dConvexPatches = m_dConvexPatches.get<uint2>();

        uint32_t sectorPadding
            = m_fullCircleNb * sizeof( uint2 ) + ( ( m_fullCircleNb * sizeof( uint2 ) ) % sizeof( float4 ) );
        float4 * dSectors = reinterpret_cast<float4 *>( m_dFCircleAndSectors.get() + sectorPadding );

        uint4 * dSegments    = m_dSegments.get<uint4>();
        uint2 * dFullCircles = m_dFCircleAndSectors.get<uint2>();

        float4 * concavePatchPositions
            = reinterpret_cast<float4 *>( m_dIntersections.get() + m_intersectionNb * sizeof( float4 ) );
        float4 * concavePatcheNeighbors = reinterpret_cast<float4 *>(
            m_dIntersections.get() + m_intersectionNb * ( sizeof( int4 ) + sizeof( float4 ) ) );

        return { m_atomNb,
                 dAtoms,

                 // Convex patch data
                 m_convexPatchNb,
                 dConvexPatches,

                 m_sectorNb,
                 dSectors,

                 // Toroidal patches
                 m_segmentNb,
                 dSegments,

                 m_fullCircleNb,
                 dFullCircles,

                 // Concave patches
                 m_intersectionNb,
                 concavePatchPositions,
                 m_dIntersections.get<int4>(),
                 concavePatcheNeighbors,

                 m_probeRadius,
                 MaxIntersectionNeighbors };
    }

    uint16_t Sesdf::getMaxNeighborPerAtom() const { return MaxNeighborPerAtom; }
    uint16_t Sesdf::getMaxIntersectionsPerCircles() const { return MaxIntersectionsPerCircles; }
    uint16_t Sesdf::getMaxIntersectionNeighbors() const { return MaxIntersectionNeighbors; }

    std::vector<uint32_t> Sesdf::getAtomIndices() { return m_dAtomIndices.toHost<uint32_t>(); }

    DeviceBuffer & Sesdf::getDAtomIndices() { return m_dAtomIndices; }
    DeviceBuffer & Sesdf::getDAtomNeighborsCount() { return m_dAtomNeighborsCount; }
    DeviceBuffer & Sesdf::getDAtomNeighborsIndices() { return m_dAtomNeighborsIndices; }
    DeviceBuffer & Sesdf::getDCircleVisibilityStatus() { return m_dCircleVisibilityStatus; }
    DeviceBuffer & Sesdf::getDGlobalToTrimmedId() { return m_dGlobalToTrimmedId; }

} // namespace bcs
