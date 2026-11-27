#pragma once

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
