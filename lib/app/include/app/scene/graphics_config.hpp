#ifndef __VTX_APP_SCENE_GRAPHICS_CONFIG__
#define __VTX_APP_SCENE_GRAPHICS_CONFIG__

#include "app/ecs.hpp"

namespace VTX::App::Scene
{

	/**
	 * @brief Current graphics config.
	 */
	struct GraphicsConfig
	{
		/**
		 * @brief Linked preset.
		 */
		ECS::Entity preset;
	};
} // namespace VTX::App::Scene

#endif
