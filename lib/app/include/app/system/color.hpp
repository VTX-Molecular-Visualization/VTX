#ifndef __VTX_APP_SYSTEM_COLOR__
#define __VTX_APP_SYSTEM_COLOR__

#include <core/struct/topology.hpp>
#include <renderer/color.hpp>
#include <unordered_map>

namespace VTX::App::System
{
	/**
	 * @brief Default values.
	 */
	constexpr Renderer::E_COLOR_SCHEME					   COLOR_SCHEME_DEFAULT = Renderer::E_COLOR_SCHEME::ATOM;
	constexpr Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE COLOR_SCHEME_SECONDARY_STRUCTURE_DEFAULT
		= Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::STRUCTURE;

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

		/**
		 * @brief Associates secondary structure color schemes with lists of residue ranges.
		 */
		std::unordered_map<Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE, Core::Struct::IndexRangeList>
			colorSchemeSecondaryStructureResidues;

		/**
		 * @brief Associates custom renderer color indexes with lists of residue ranges.
		 */
		std::unordered_map<Renderer::ColorIndex, Core::Struct::IndexRangeList> customSecondaryStructureColorResidues;
	};

} // namespace VTX::App::System

#endif
