#ifndef __VTX_RENDERER_GEOMETRY_SES_CUDA__
#define __VTX_RENDERER_GEOMETRY_SES_CUDA__

#include <array>
#include <core/chemdb/atom.hpp>
#include <cstdint>
#include <memory>
#include <span>
#include <util/types.hpp>
#include <vector>

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

		uint32_t convexPatchNb	= 0;
		uint32_t circlePatchNb	= 0;
		uint32_t segmentPatchNb = 0;
		uint32_t concavePatchNb = 0;

		std::vector<UVec2> convexPatches;
		std::vector<UVec2> circlePatches;
		std::vector<UVec4> segmentPatches;
	};

	CudaBuildResult buildCudaConstruction(
		std::span<const Vec3f>						p_positions,
		std::span<const Core::ChemDB::Atom::SYMBOL> p_symbols,
		float										p_probeRadius
	);
} // namespace VTX::Renderer::Geometry::SESDetail

#endif
