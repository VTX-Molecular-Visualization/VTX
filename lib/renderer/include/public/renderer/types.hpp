#ifndef __VTX_RENDERER_TYPES__
#define __VTX_RENDERER_TYPES__

#include <map>
#include <util/math/range_list.hpp>

namespace VTX::Renderer
{
	using ColorIndex		  = uint8_t;
	using RepresentationIndex = uint8_t;
	using ModelIndex		  = uint16_t;
	using Flag				  = uint8_t;

	using IndexRange	   = Util::Math::Range<Index>;
	using IndexRangeList   = Util::Math::RangeList<Index>;
	using IndexRangeVector = std::vector<IndexRange>;
	using MapUIDRange	   = std::map<SystemUID, IndexRange>;
	using MapUIDRangeList  = std::map<SystemUID, IndexRangeList>;

} // namespace VTX::Renderer

#endif
