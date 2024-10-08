#include "app/entity/scene/camera_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Scene
{
	void CameraEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		MAIN_REGISTRY().addComponent<Component::Scene::Transform>( p_entity );
		MAIN_REGISTRY().addComponent<Component::Render::Camera>( p_entity );
	}

	void CameraEntityBuilder::setup( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData )
	{
		Component::Render::Camera & camera = MAIN_REGISTRY().getComponent<Component::Render::Camera>( p_entity );
		camera.setupProxy();
	}
} // namespace VTX::App::Entity::Scene
