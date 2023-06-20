#ifndef __VTX_APP_ECS_BUILDING_SETUP_ENTITY_DIRECTOR__
#define __VTX_APP_ECS_BUILDING_SETUP_ENTITY_DIRECTOR__

#include "app/ecs/building/entity_director.hpp"

namespace VTX::App::ECS::Building
{
	inline const EntityDirectorID SCENE_ENTITY_ID	 = "Scene";
	inline const EntityDirectorID MOLECULE_ENTITY_ID = "Molecule";

	void setupEntityDirector();

} // namespace VTX::App::ECS::Building
#endif
