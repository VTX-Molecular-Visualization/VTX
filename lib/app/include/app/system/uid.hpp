#ifndef __VTX_APP_SYSTEM_UID__
#define __VTX_APP_SYSTEM_UID__

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	struct UID
	{
		static inline RootIndex COUNTER = 0;

		RootIndex					 system;
		Util::Math::RangeList<Index> residues;
		Util::Math::RangeList<Index> atoms;
	};
} // namespace VTX::App::System

#endif
