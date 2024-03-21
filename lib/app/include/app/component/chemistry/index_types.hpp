#ifndef __VTX_APP_COMPONENT_CHEMISTRY_INDEX_TYPES__
#define __VTX_APP_COMPONENT_CHEMISTRY_INDEX_TYPES__

#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	using AtomIndexRange	 = Util::Math::Range<atom_index_t>;
	using AtomIndexRangeList = Util::Math::RangeList<atom_index_t>;

	using ResidueIndexRange		= Util::Math::Range<size_t>;
	using ResidueIndexRangeList = Util::Math::RangeList<size_t>;

	using ChainIndexRange	  = Util::Math::Range<size_t>;
	using ChainIndexRangeList = Util::Math::RangeList<size_t>;

} // namespace VTX::App::Component::Chemistry

#endif
