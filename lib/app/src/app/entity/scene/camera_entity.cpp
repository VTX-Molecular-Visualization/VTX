#include "app/entity/scene/camera_entity.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Scene
{
	void CameraEntity::setup()
	{
		ECS_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		auto & camera = ECS_REGISTRY().addComponent<Component::Render::Camera>( *this );
		camera.setupProxy();
	}
} // namespace VTX::App::Entity::Scene
