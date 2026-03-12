#ifndef __VTX_APP_SYSTEM_SELECTION__
#define __VTX_APP_SYSTEM_SELECTION__

#include <core/struct/system.hpp>
#include <util/math/bitset.hpp>

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
		Util::Math::BitSet atoms;
	};

} // namespace VTX::App::System

#endif
