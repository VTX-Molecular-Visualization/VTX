#ifndef __VTX_APP_SYSTEM_DELETED__
#define __VTX_APP_SYSTEM_DELETED

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	struct Deleted
	{
		Util::Math::RangeList<Index> chains;
		Util::Math::RangeList<Index> residues;
		Util::Math::RangeList<Index> atoms;
	};
} // namespace VTX::App::System

#endif
