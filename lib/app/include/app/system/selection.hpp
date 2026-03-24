#ifndef __VTX_APP_SYSTEM_SELECTION__
#define __VTX_APP_SYSTEM_SELECTION__

#include <core/struct/topology.hpp>
#include <util/math/bitset.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Selection state of an item.
	 */
	enum struct E_SELECTION_STATE : uint8_t
	{
		NONE,
		PARTIAL,
		FULL
	};

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
