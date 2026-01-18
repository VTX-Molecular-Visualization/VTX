#ifndef __VTX_APP_SYSTEM_REPRESENTATION__
#define __VTX_APP_SYSTEM_REPRESENTATION

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Store representation informations.
	 */
	struct Representation
	{
		/**
		 * @brief Linked preset.
		 */
		ECS::Entity preset;

		/**
		 * @brief Atoms concerned.
		 */
		Core::Struct::IndexRangeList atoms;
	};
} // namespace VTX::App::System

#endif
