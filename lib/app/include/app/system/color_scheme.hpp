#ifndef __VTX_APP_SYSTEM_COLOR_SCHEME__
#define __VTX_APP_SYSTEM_COLOR_SCHEME

#include <core/struct/system.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Different color schemes to apply.
	 */
	enum struct E_COLOR_SCHEME : uint8_t
	{
		MOLECULE,
		CHAIN,
		RESIDUE,
		ATOM,
		CUSTOM
	};

	constexpr E_COLOR_SCHEME COLOR_SCHEME_DEFAULT = E_COLOR_SCHEME::ATOM;

	/**
	 * @brief Store color scheme informations.
	 */
	struct ColorScheme
	{
		/**
		 * @brief Scheme.
		 */
		E_COLOR_SCHEME scheme = COLOR_SCHEME_DEFAULT;

		/**
		 * @brief Residues concerned.
		 */
		Core::Struct::IndexRangeList residues;
	};
} // namespace VTX::App::System

#endif
