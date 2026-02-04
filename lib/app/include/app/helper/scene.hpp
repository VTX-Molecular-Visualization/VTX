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
	std::unordered_map<SystemUID, ECS::Entity> getAllSystemsMap();

	/**
	 * @brief Find system entity by UID.
	 */
	std::optional<ECS::Entity> findSystemByRootUID( const SystemUID );
	std::optional<ECS::Entity> findSystemByAtomUID( const PickingUID );
	std::optional<ECS::Entity> findSystemByResidueUID( const PickingUID );

} // namespace VTX::App::Helper::Scene

#endif
