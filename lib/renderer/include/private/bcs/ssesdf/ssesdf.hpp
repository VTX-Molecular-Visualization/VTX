#ifndef BCS_SSESDF_SSESDF_HPP
#define BCS_SSESDF_SSESDF_HPP

#include "bcs/core/molecule.hpp"
#include "bcs/cuda/grid.cuh"
#include "bcs/ssesdf/graphics.hpp"

#ifndef BCS_SSESDF_MAXNEIGHBORPERATOM
#define BCS_SSESDF_MAXNEIGHBORPERATOM 128u
#endif // BCS_SSESDF_MAXNEIGHBORPERATOM

namespace bcs
{
    class Ssesdf
    {
      public:
        Ssesdf( ConstSpan<Vec4f> molecule,
                const Aabb &     aabb,
                const float      probeRadius  = 1.4f,
                bool             buildSurface = true,
                bool             graphics     = true );

        Ssesdf( const Ssesdf & )             = delete;
        Ssesdf & operator=( const Ssesdf & ) = delete;

        Ssesdf( Ssesdf && );
        Ssesdf & operator=( Ssesdf && );

        ~Ssesdf();

        void build();

        ssesdf::SsesdfGraphics getGraphics() const;
        ssesdf::SsesdfData     getData();

        // Helper to analyse the surface
        uint16_t              getMaxNeighborPerAtom() const;
        uint16_t              getMaxIntersectionsPerCircles() const;
        uint16_t              getMaxIntersectionNeighbors() const;
        std::vector<uint32_t> getAtomIndices();
        DeviceBuffer &        getDAtomIndices();
        DeviceBuffer &        getDAtomNeighborsCount();
        DeviceBuffer &        getDAtomNeighborsIndices();
        DeviceBuffer &        getDCircleVisibilityStatus();

      private:
        static const uint16_t MaxNeighborPerAtom;
        static const uint16_t MaxIntersectionsPerCircles;
        static const uint16_t MaxIntersectionNeighbors;

        ConstSpan<Vec4f> m_molecule;
        float            m_probeRadius = 0.f;
        uint32_t         m_atomNb      = 0;
        bool             m_graphics    = true;

        uint32_t m_intersectionNb             = 0;
        uint32_t m_intersectionWithNeighborNb = 0;
        uint32_t m_circleNb                   = 0;

        GridInfo m_gridConfiguration {};

        // Preallocated buffers
        DeviceBuffer     m_dAtomIndices;
        DeviceBuffer     m_dAtomNeighborsCount;
        DeviceBuffer     m_dAtomNeighborsIndices;
        DeviceBuffer     m_dCircleVisibilityStatus;
        AccelerationGrid m_accelerationGrid;

        uint32_t * m_dIntersectionNb  = nullptr;
        uint32_t * m_hIntersectionNb  = nullptr;
        uint32_t * m_hVisibleCircleNb = nullptr;

        // Rendering buffers
        ResultBuffer m_dAtoms;
        ResultBuffer m_dCircles;
        ResultBuffer m_dIntersections;
    };

} // namespace bcs

#endif // BCS_SSESDF_SSESDF_HPP
