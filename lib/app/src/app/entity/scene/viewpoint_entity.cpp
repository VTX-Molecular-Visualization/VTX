#include "app/entity/scene/viewpoint_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/viewpoint.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/entity/scene/scene_item_entity.hpp"
#include "app/vtx_app.hpp"
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Entity::Scene
{
	void ViewpointEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::addComponent( p_entity, p_extraData );

		MAIN_REGISTRY().addComponent<Component::Render::Viewpoint>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Scene::Selectable>( p_entity );
	}
	void ViewpointEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData )
	{
		SceneItemEntityBuilder::setup( p_entity, p_extraData );

		Component::Scene::SceneItemComponent & sceneItemComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( p_entity );
		sceneItemComponent.setName( "Viewpoint" );
	}
	void ViewpointEntityBuilder::postSetup(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		SceneItemEntityBuilder::postSetup( p_entity, p_extraData );
	}
} // namespace VTX::App::Entity::Scene
