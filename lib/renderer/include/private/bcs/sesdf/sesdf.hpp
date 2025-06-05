#ifndef BCS_SESDF_SESDF_HPP
#define BCS_SESDF_SESDF_HPP

#include "bcs/core/type.hpp"
#include "bcs/cuda/grid.cuh"
#include "bcs/sesdf/graphics.hpp"

#ifndef BCS_SESDF_MAXNEIGHBORPERATOM
#define BCS_SESDF_MAXNEIGHBORPERATOM 128u
#endif // BCS_SESDF_MAXNEIGHBORPERATOM

namespace bcs
{
	class Sesdf
	{
	  public:
		Sesdf(
			ConstSpan<Vec4f> molecule,
			const Aabb &	 aabb,
			const float		 probeRadius  = 1.4f,
			bool			 buildSurface = true,
			bool			 graphics	  = true
		);

		Sesdf( const Sesdf & )			   = delete;
		Sesdf & operator=( const Sesdf & ) = delete;

		Sesdf( Sesdf && );
		Sesdf & operator=( Sesdf && );

		~Sesdf();

		void build();

		sesdf::SesdfGraphics getGraphics() const;
		sesdf::SesdfData	 getData();

		// Helper to analyse the surface
		uint16_t			  getMaxNeighborPerAtom() const;
		uint16_t			  getMaxIntersectionsPerCircles() const;
		uint16_t			  getMaxIntersectionNeighbors() const;
		std::vector<uint32_t> getAtomIndices();
		DeviceBuffer &		  getDAtomIndices();
		DeviceBuffer &		  getDAtomNeighborsCount();
		DeviceBuffer &		  getDAtomNeighborsIndices();
		DeviceBuffer &		  getDCircleVisibilityStatus();
		DeviceBuffer &		  getDGlobalToTrimmedId();

	  private:
		static const uint16_t MaxNeighborPerAtom;
		static const uint16_t MaxMeanIntersectionsPerCircles;
		static const uint16_t MaxIntersectionsPerCircles;
		static const uint16_t MaxIntersectionNeighbors;

		ConstSpan<Vec4f> m_molecule;
		float			 m_probeRadius = 0.f;
		uint32_t		 m_atomNb	   = 0;
		bool			 m_graphics	   = true;

		uint32_t m_convexPatchNb			  = 0;
		uint32_t m_segmentNb				  = 0;
		uint32_t m_intersectionNb			  = 0;
		uint32_t m_intersectionWithNeighborNb = 0;
		uint32_t m_fullCircleNb				  = 0;
		uint32_t m_sectorNb					  = 0;

		GridInfo m_gridConfiguration {};

		// Preallocated buffers
		DeviceBuffer	 m_dAtomIndices;
		DeviceBuffer	 m_dAtomNeighborsCount;
		DeviceBuffer	 m_dAtomNeighborsIndices;
		DeviceBuffer	 m_dCircleVisibilityStatus;
		DeviceBuffer	 m_dGlobalToTrimmedId;
		AccelerationGrid m_accelerationGrid;

		uint32_t * m_dIntersectedCircleNb = nullptr;
		uint32_t * m_hIntersectedCircleNb = nullptr;
		uint32_t * m_dIntersectionNb	  = nullptr;
		uint32_t * m_hIntersectionNb	  = nullptr;
		uint32_t * m_dFullCircleNb		  = nullptr;
		uint32_t * m_hFullCircleNb		  = nullptr;
		uint32_t * m_dSegmentCount		  = nullptr;
		uint32_t * m_hSegmentCount		  = nullptr;
		uint32_t * m_hVisibleCircleNb	  = nullptr;

		// Rendering buffers
		ResultBuffer m_dAtoms;
		ResultBuffer m_dIntersections;
		ResultBuffer m_dSegments;
		ResultBuffer m_dConvexPatches;
		ResultBuffer m_dFCircleAndSectors;
	};

} // namespace bcs

#endif // BCS_SESDF_SESDF_HPP
