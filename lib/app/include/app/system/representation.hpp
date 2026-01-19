#ifndef __VTX_APP_SYSTEM_REPRESENTATION__
#define __VTX_APP_SYSTEM_REPRESENTATION__

#include <core/struct/system.hpp>
#include <renderer/representation.hpp>
#include <unordered_map>

namespace VTX::App::System
{

	/**
	 * @brief Store atom representation index.
	 */
	struct Representation
	{
		/**
		 * @brief Associates entities with lists of index ranges.
		 */
		std::unordered_map<ECS::Entity, Core::Struct::IndexRangeList> presetAtoms;

		/**
		 * @brief Atom representation.
		 */
		std::vector<Renderer::RepresentationIndex> atoms;
	};

} // namespace VTX::App::System

#endif
