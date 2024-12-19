#include "app/entity/camera.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/component/controller.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity
{
	void Camera::setup()
	{
		ECS_REGISTRY().addComponent<Component::Scene::Transform>( *this );
		auto & camera = ECS_REGISTRY().addComponent<Component::Render::Camera>( *this );
		camera.setupProxy();
		ECS_REGISTRY().addComponent<Component::Controller>( *this );
	}
} // namespace VTX::App::Entity
