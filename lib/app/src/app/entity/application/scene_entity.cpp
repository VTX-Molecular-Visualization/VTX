#include "app/entity/application/scene_entity.hpp"
#include "app/application/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"

namespace VTX::App::Entity::Application
{
	void SceneEntityBuilder::addComponent( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		App::Application::MAIN_REGISTRY().addComponent<App::Application::Scene>( p_entity );
		App::Application::MAIN_REGISTRY().addComponent<App::Component::IO::SceneFileInfo>( p_entity );
	}

	void SceneEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData ) {}
} // namespace VTX::App::Entity::Application
