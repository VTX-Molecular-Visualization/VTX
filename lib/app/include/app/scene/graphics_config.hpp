#ifndef VTX_APP_SCENE_GRAPHICS_CONFIG_HPP
#define VTX_APP_SCENE_GRAPHICS_CONFIG_HPP

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
		Entity preset;
	};
} // namespace VTX::App::Scene

#endif
