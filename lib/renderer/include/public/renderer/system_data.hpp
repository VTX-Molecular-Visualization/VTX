#ifndef __VTX_RENDERER_SYSTEM_DATA__
#define __VTX_RENDERER_SYSTEM_DATA__

#include "renderer/types.hpp"
#include <core/struct/topology.hpp>
#include <span>
#include <util/math/bitset.hpp>
#include <vector>

namespace VTX::Renderer
{
	struct SystemData
	{
		const SystemUID				   uid;
		const Mat4f					   transform;
		const Core::Struct::Topology & data;
		std::span<const Vec3f>		   trajectory;

		// TODO: delete and use ssbo with atom indexes in shader.
		const std::vector<PickingUID> atomUids;
		const std::vector<PickingUID> residueUids;

		const std::vector<ColorIndex>		   atomColors;
		const MapRepresentationRanges		   representationRanges;
		const std::vector<RepresentationIndex> atomRepresentations;
		const Util::Math::BitSet			   visibility;
		const std::vector<Flag>				   atomFlags;
	};
} // namespace VTX::Renderer

#endif
