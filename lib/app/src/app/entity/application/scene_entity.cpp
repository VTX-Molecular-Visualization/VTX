#include "app/entity/application/scene_entity.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity::Application
{
	void SceneEntity::setup()
	{
		MAIN_REGISTRY().addComponent<App::Application::Scene>( *this );
		MAIN_REGISTRY().addComponent<App::Component::IO::SceneFileInfo>( *this );
	}
} // namespace VTX::App::Entity::Application
