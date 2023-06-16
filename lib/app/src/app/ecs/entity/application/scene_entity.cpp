#include "app/ecs/entity/application/scene_entity.hpp"
#include "app/application/scene.hpp"
#include "app/ecs/registry_manager.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::ECS::Entity::Application
{
	Core::BaseEntity generateSceneEntity()
	{
		ECS::Core::BaseEntity sceneEntity = ECS::MAIN_REGISTRY().createEntity();
		ECS::MAIN_REGISTRY().addComponent<App::Application::Scene>( sceneEntity );

		return sceneEntity;
	}

} // namespace VTX::App::ECS::Entity::Application
