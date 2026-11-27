#pragma once

#include "app/ecs.hpp"

namespace VTX::App::Scene
{

	/**
	 * @brief Current material.
	 */
	struct Material
	{
		/**
		 * @brief Linked preset.
		 */
		Entity preset;
	};
} // namespace VTX::App::Scene
