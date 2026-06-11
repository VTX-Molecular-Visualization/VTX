#ifndef __VTX_APP_SYSTEM_COLOR__
#define __VTX_APP_SYSTEM_COLOR__

#include <core/struct/topology.hpp>
#include <renderer/color.hpp>

namespace VTX::App::System
{

	constexpr Renderer::E_COLOR_SCHEME COLOR_SCHEME_DEFAULT = Renderer::E_COLOR_SCHEME::ATOM;

	/**
	 * @brief Store atom color index in the current layout.
	 */
	struct Color
	{
		/**
		 * @brief Associates color schemes with lists of index ranges.
		 */
		std::unordered_map<Renderer::E_COLOR_SCHEME, Core::Struct::IndexRangeList> colorSchemeAtoms;

		/**
		 * @brief Associates custom renderer color indexes with lists of atom ranges.
		 */
		std::unordered_map<Renderer::ColorIndex, Core::Struct::IndexRangeList> customColorAtoms;

		/**
		 * @brief Associates custom renderer color indexes for carbon atoms with lists of atom ranges.

		 */
		std::unordered_map<Renderer::ColorIndex, Core::Struct::IndexRangeList> carbonCustomColorAtoms;
	};

} // namespace VTX::App::System

#endif
