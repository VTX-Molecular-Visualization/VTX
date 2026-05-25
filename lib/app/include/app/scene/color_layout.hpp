#ifndef __VTX_APP_SCENE_COLOR_LAYOUT__
#define __VTX_APP_SCENE_COLOR_LAYOUT__

#include "app/ecs.hpp"

namespace VTX::App::Scene
{

	/**
	 * @brief Current color layout.
	 */
	struct ColorLayout
	{
		/**
		 * @brief Linked preset.
		 */
		Entity preset;
	};
} // namespace VTX::App::Scene

#endif
