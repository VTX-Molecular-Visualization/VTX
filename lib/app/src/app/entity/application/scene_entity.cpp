#include "app/entity/application/scene_entity.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Application
{
	void SceneEntityBuilder::addComponent(
		const Core::ECS::BaseEntity & p_entity,
		const Util::VariantMap &	  p_extraData
	)
	{
		MAIN_REGISTRY().addComponent<App::Application::Scene>( p_entity );
		MAIN_REGISTRY().addComponent<App::Component::IO::SceneFileInfo>( p_entity );
	}
} // namespace VTX::App::Entity::Application
