#ifndef __VTX_APP_SYSTEM_COLOR__
#define __VTX_APP_SYSTEM_COLOR__

#include <core/struct/system.hpp>
#include <renderer/color.hpp>

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
	 * @brief Store atom color index in the current layout.
	 */
	struct Color
	{
		/**
		 * @brief Associates color schemes with lists of index ranges.
		 */
		std::unordered_map<E_COLOR_SCHEME, Core::Struct::IndexRangeList> colorSchemeAtoms;

		/**
		 * @brief Atom colors.
		 */
		std::vector<Renderer::Color::ColorIndex> atoms;
	};

} // namespace VTX::App::System

#endif
