#ifndef __VTX_RENDERER_GEOMETRY_SES_CUDA__
#define __VTX_RENDERER_GEOMETRY_SES_CUDA__

#include <array>
#include <core/chemdb/atom.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <util/types.hpp>

namespace VTX::Renderer::Geometry::SESDetail
{
	using UVec2 = std::array<uint32_t, 2>;
	using UVec4 = std::array<uint32_t, 4>;

	struct CudaConstruction;

	struct CudaConstructionDeleter
	{
		void operator()( CudaConstruction * p_construction ) const;
	};

	using CudaConstructionPtr = std::unique_ptr<CudaConstruction, CudaConstructionDeleter>;

	struct CudaBuildResult
	{
		CudaConstructionPtr construction;

		uint32_t atomNb			= 0;
		uint32_t convexPatchNb	= 0;
		uint32_t circlePatchNb	= 0;
		uint32_t segmentPatchNb = 0;
		uint32_t probeNb		= 0;
		uint32_t sectorNb		= 0;
		uint32_t concavePatchNb = 0;
	};

	// GPU buffer slice owned by the render graph and mapped for CUDA access.
	struct CudaBufferView
	{
		void * devicePtr   = nullptr;
		size_t sizeBytes   = 0;
		size_t offsetBytes = 0;
	};

	// Output contract for the SES renderer buffers.
	//
	// Types:
	// - atoms: float4(x, y, z, radius)
	// - atomIds: uint renderer atom id for each BCS/sorted atom id
	// - sectors: float4(axis.xyz, angle)
	// - convexPatches: uint2(firstSector, endSector)
	// - circlePatches: uint2(atomA, atomB)
	// - segmentPatches: uint4(atomA, atomB, probeA, probeB)
	// - probes: float4(x, y, z, signedSide)
	// - probeAtomIndices: int4(atomA, atomB, atomC, neighborCount)
	// - probeNeighbors: float4 neighbor positions, laid out as probeId * maxProbeNeighborNb + neighborId
	struct SesdfOutputBuffers
	{
		CudaBufferView atoms;
		CudaBufferView atomIds;
		CudaBufferView sectors;
		CudaBufferView convexPatches;
		CudaBufferView circlePatches;
		CudaBufferView segmentPatches;
		CudaBufferView probes;
		CudaBufferView probeAtomIndices;
		CudaBufferView probeNeighbors;

		uint32_t atomIndexOffset		   = 0;
		uint32_t rendererAtomIndexOffset = 0;
		uint32_t probeIndexOffset	   = 0;
		uint32_t sectorIndexOffset	   = 0;
		uint32_t maxProbeNeighborNb	   = 0;
	};

	struct SesdfInputBuffers
	{
		CudaBufferView positions;
		CudaBufferView symbols;

		uint32_t atomOffset = 0;
		uint32_t atomNb		= 0;
	};

	CudaBuildResult buildCudaConstruction(
		std::span<const Vec3f>						p_positions,
		std::span<const Core::ChemDB::Atom::SYMBOL> p_symbols,
		float										p_probeRadius
	);

	CudaBuildResult buildCudaConstructionFromRendererBuffers(
		const SesdfInputBuffers & p_inputs,
		std::span<const Vec3f>	p_aabbPositions,
		float					p_probeRadius
	);

	void writeCudaConstruction( CudaConstruction &, const SesdfOutputBuffers & );
} // namespace VTX::Renderer::Geometry::SESDetail

#endif
