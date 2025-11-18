#ifndef __VTX_APP_SYSTEM_VISIBILITY__
#define __VTX_APP_SYSTEM_VISIBILITY

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	struct Visibility
	{
		bool						 isFullSystemVisible = true;
		Util::Math::RangeList<Index> chains;
		Util::Math::RangeList<Index> residues;
		Util::Math::RangeList<Index> atoms;
	};
} // namespace VTX::App::System

#endif
