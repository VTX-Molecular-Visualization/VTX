#ifndef __VTX_APP_ENTITY_ALL_ENTITIES__
#define __VTX_APP_ENTITY_ALL_ENTITIES__

#include "app/application/ecs/entity_director.hpp"

namespace VTX::App::Entity
{
	inline const App::Application::ECS::EntityDirectorID SCENE_ENTITY_ID	 = "Scene";
	inline const App::Application::ECS::EntityDirectorID CAMERA_ENTITY_ID	 = "Camera";
	inline const App::Application::ECS::EntityDirectorID MOLECULE_ENTITY_ID	 = "Molecule";
	inline const App::Application::ECS::EntityDirectorID VIEWPOINT_ENTITY_ID = "Viewpoint";
} // namespace VTX::App::Entity
#endif
