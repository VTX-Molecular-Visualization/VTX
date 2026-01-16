#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/descriptors.hpp"
#include <util/math/range_list.hpp>

namespace VTX::Renderer::Geometry
{
	using RangeList			= Util::Math::RangeList<size_t>;
	using DrawRangeArray	= Desc::DrawCall::RangeArrays;
	using DrawRangeElements = Desc::DrawCall::RangeElements;

	struct BaseGeometry
	{
		bool show = true;
	};
} // namespace VTX::Renderer::Geometry

#endif
