#ifndef VTX_APP_SCENE_MATERIAL_HPP
#define VTX_APP_SCENE_MATERIAL_HPP

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

#endif
