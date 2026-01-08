#ifndef __VTX_APP_PRESET_INSTANCE__
#define __VTX_APP_PRESET_INSTANCE__

#include "app/ecs.hpp"

namespace VTX::App::Preset
{
	/**
	 * @brief Define a preset instance.
	 */
	template<typename T>
	struct Instance
	{
		ECS::Entity entity;
	};
} // namespace VTX::App::Preset

#endif
