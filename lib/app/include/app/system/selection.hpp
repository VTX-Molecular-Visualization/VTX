#ifndef __VTX_APP_SYSTEM_SELECTION__
#define __VTX_APP_SYSTEM_SELECTION

#include <core/struct/system.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Store selected elements.
	 */
	struct Selection
	{
		/**
		 * @brief Selected atoms.
		 */
		Core::Struct::IndexRangeList atoms;
	};

} // namespace VTX::App::System

#endif
