#ifndef __VTX_APP_SYSTEM_VISIBILITY__
#define __VTX_APP_SYSTEM_VISIBILITY__

#include <core/struct/system.hpp>
#include <util/math/bitset.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Visibility state of an item.
	 */
	enum struct E_VISIBLE_STATE : uint8_t
	{
		VISIBLE,
		HIDDEN,
		PARTIAL
	};

	/**
	 * @brief Store visible elements.
	 */
	struct Visibility
	{
		/**
		 * @brief Visible atoms.
		 */
		Util::Math::BitSet atoms;
	};

} // namespace VTX::App::System

#endif
