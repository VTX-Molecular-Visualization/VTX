#ifndef __VTX_APP_HELPER_SCENE__
#define __VTX_APP_HELPER_SCENE__

#include "app/ecs.hpp"
#include <optional>
#include <util/uid.hpp>

namespace VTX::App::Helper::Scene
{
	/**
	 * @brief Get all system entities.
	 */
	std::vector<Entity> getAllSystems();

	/**
	 * @brief Find system by element UID.
	 */
	std::optional<Entity> findSystemByAtomUID( const UID32 );
	std::optional<Entity> findSystemByResidueUID( const UID32 );

} // namespace VTX::App::Helper::Scene

#endif
