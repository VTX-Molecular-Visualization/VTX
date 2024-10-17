#include "app/entity/viewpoint.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/viewpoint.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::App::Entity
{
	void Viewpoint::setup()
	{
		auto & sceneItemComponent = ECS_REGISTRY().addComponent<Component::Scene::SceneItemComponent>( *this );

		ECS_REGISTRY().addComponent<Component::Render::Viewpoint>( *this );
		ECS_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		ECS_REGISTRY().addComponent<Component::Scene::Selectable>( *this );

		sceneItemComponent.setName( "Viewpoint" );
	}

} // namespace VTX::App::Entity
