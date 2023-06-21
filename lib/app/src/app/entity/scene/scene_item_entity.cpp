#include "app/entity/scene/scene_item_entity.hpp"
#include "app/application/scene.hpp"

namespace VTX::App::Entity::Scene
{
	void SceneItemEntityBuilder::addComponent( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData )
	{
		Application::Scene * const scenePtr = p_extraData.at( "scene" ).getPtr<Application::Scene>();

		Application::MAIN_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( p_entity );
	}

	void SceneItemEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData ) {}
} // namespace VTX::App::Entity::Scene
