#include "app/ecs/entity/scene/scene_item_entity.hpp"
#include "app/application/scene.hpp"

namespace VTX::App::ECS::Entity::Scene
{
	void SceneItemEntityBuilder::addComponent( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		Application::Scene * const scenePtr = p_extraData.at( "scene" ).getPtr<Application::Scene>();

		ECS::MAIN_REGISTRY().addComponent<ECS::Component::SceneItemComponent>( p_entity );
	}

	void SceneItemEntityBuilder::setup( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData ) {}
} // namespace VTX::App::ECS::Entity::Scene
