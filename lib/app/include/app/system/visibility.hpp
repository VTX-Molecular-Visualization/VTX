#ifndef __VTX_APP_SYSTEM_VISIBILITY__
#define __VTX_APP_SYSTEM_VISIBILITY__

#include <core/struct/system.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Store visible elements.
	 */
	struct Visibility
	{
		/**
		 * @brief Visible atoms.
		 */
		Core::Struct::IndexRangeList atoms;
	};

} // namespace VTX::App::System

#endif
