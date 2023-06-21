#ifndef __VTX_APP_INTERNAL_ECS_SETUP_ENTITY_DIRECTOR__
#define __VTX_APP_INTERNAL_ECS_SETUP_ENTITY_DIRECTOR__

#include "app/application/ecs/building/entity_director.hpp"

namespace VTX::App::Internal::ECS
{
	inline const Application::ECS::Building::EntityDirectorID SCENE_ENTITY_ID	 = "Scene";
	inline const Application::ECS::Building::EntityDirectorID MOLECULE_ENTITY_ID = "Molecule";

	void setupEntityDirector();

} // namespace VTX::App::Internal::ECS
#endif
