#ifndef __VTX_RENDERER_TYPES__
#define __VTX_RENDERER_TYPES__

#include <core/chemdb/atom.hpp>
#include <map>
#include <util/math/range_list.hpp>

namespace VTX::Renderer
{
	using Symbol			  = Core::ChemDB::Atom::SYMBOL;
	using ColorIndex		  = uint8_t;
	using RepresentationIndex = uint8_t;
	using Flag				  = uint8_t;

	using IndexRange	 = Util::Math::Range<Index>;
	using IndexRangeList = Util::Math::RangeList<Index>;

} // namespace VTX::Renderer

#endif
