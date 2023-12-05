#include "app/entity/scene/camera_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/component/render/camera.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Scene
{
	void CameraEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		MAIN_REGISTRY().addComponent<Component::Render::Camera>( p_entity );
	}
} // namespace VTX::App::Entity::Scene
