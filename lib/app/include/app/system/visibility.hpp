#pragma once

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

	bool isAnythingVisible();

} // namespace VTX::App::System
