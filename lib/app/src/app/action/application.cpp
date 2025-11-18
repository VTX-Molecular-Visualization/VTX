#include "app/action/application.hpp"
#include "app/ecs.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include <renderer/facade.hpp>

namespace VTX::App::Action::Application
{
	/*
	void NewScene::execute() { SCENE().reset(); }
	*/

	void Quit::execute()
	{
		// APP::onEndOfFrameOneShot += []() { APP::stop(); };
	}

	void Resize::execute( const size_t p_width, const size_t p_height )
	{
		REG().patch<Scene::Camera>(
			ECS::getFirstEntityOnlyWithComponents<Scene::Camera>(),
			[ p_width, p_height ]( Scene::Camera & p_camera )
			{
				p_camera.screenHeight = p_height;
				p_camera.screenWidth  = p_width;
			}
		);

		RENDERER().resize( p_width, p_height );
	}

} // namespace VTX::App::Action::Application
