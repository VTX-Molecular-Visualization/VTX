#include "app/ecs/entity/application/scene_entity.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/ecs/registry_manager.hpp"

namespace VTX::App::ECS::Entity::Application
{
	void SceneEntityBuilder::addComponent( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		ECS::MAIN_REGISTRY().addComponent<App::Application::Scene>( p_entity );
		ECS::MAIN_REGISTRY().addComponent<App::Component::IO::SceneFileInfo>( p_entity );
	}

	void SceneEntityBuilder::setup( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData ) {}
} // namespace VTX::App::ECS::Entity::Application
