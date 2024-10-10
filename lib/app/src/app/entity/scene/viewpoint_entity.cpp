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
	void ViewpointEntity::setup()
	{
		auto & sceneItemComponent = MAIN_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		MAIN_REGISTRY().addComponent<Component::Render::Viewpoint>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		MAIN_REGISTRY().addComponent<Component::Scene::Selectable>( *this );

		sceneItemComponent.setName( "Viewpoint" );
	}

} // namespace VTX::App::Entity::Scene
