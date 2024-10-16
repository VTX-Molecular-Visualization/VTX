#include "app/entity/scene.hpp"
#include "app/application/scene.hpp"
#include "app/component/io/scene_file_info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Entity
{
	void Scene::setup()
	{
		ECS_REGISTRY().addComponent<App::Application::Scene>( *this );
		ECS_REGISTRY().addComponent<App::Component::IO::SceneFileInfo>( *this );
	}
} // namespace VTX::App::Entity
