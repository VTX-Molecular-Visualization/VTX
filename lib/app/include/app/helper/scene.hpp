#ifndef __VTX_APP_HELPER_SCENE__
#define __VTX_APP_HELPER_SCENE__

#include "app/ecs.hpp"
#include <optional>
#include <util/types.hpp>

namespace VTX::App::Helper::Scene
{
	/**
	 * @brief Get all system entities.
	 */
	std::vector<ECS::Entity> getAllSystems();

	/**
	 * @brief Get a map of all systems with their root UIDs as keys.
	 */
	std::unordered_map<RootUID, ECS::Entity> getAllSystemsMap();

	/**
	 * @brief Find system entity by its root UID.
	 */
	std::optional<ECS::Entity> findSystemByRootUID( const RootUID );

} // namespace VTX::App::Helper::Scene

#endif
