#ifndef __VTX_RENDERER_SYSTEM_DATA__
#define __VTX_RENDERER_SYSTEM_DATA__

#include <core/struct/system.hpp>

namespace VTX::Renderer
{
	struct SystemData
	{
		const SystemUID				 uid;
		const Mat4f &				 transform;
		const Core::Struct::System & data;
		std::span<const Vec3f>		 trajectory;

		// TODO: delete and use ssbo with atom indexes in shader.
		const std::vector<PickingUID> atomUids;
		const std::vector<PickingUID> residueUids;

		// const std::vector<ColorIndex>												colorIndexes;
		// const std::unordered_map<RepresentationIndex, Core::Struct::IndexRangeList> representationRanges;
		// const std::vector<std::byte>												visibleAtoms;
		// const std::vector<std::byte>												selectedAtoms;
	};
} // namespace VTX::Renderer

#endif
