#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include <map>
#include <util/math/range_list.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Geometry
{
	using IndexRange		= Util::Math::Range<Index>;
	using IndexRangeList	= Util::Math::RangeList<Index>;
	using MapUIDRange		= std::map<SystemUID, IndexRange>;
	using DrawRangeArray	= Desc::DrawCall::RangeArrays;
	using DrawRangeElements = Desc::DrawCall::RangeElements;

	struct BaseGeometry
	{
	};
} // namespace VTX::Renderer::Geometry

#endif
