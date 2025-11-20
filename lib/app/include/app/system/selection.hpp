#ifndef __VTX_APP_SYSTEM_SELECTION__
#define __VTX_APP_SYSTEM_SELECTION

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Store selected elements.
	 */
	struct Selection
	{
		bool						 isFullSystemSelected = false;
		Util::Math::RangeList<Index> chains;
		Util::Math::RangeList<Index> residues;
		Util::Math::RangeList<Index> atoms;
	};
} // namespace VTX::App::System

#endif
