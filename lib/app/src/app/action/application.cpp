#include "app/action/application.hpp"
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

	void Resize::execute()
	{
		auto view = REG().view<Scene::Camera>();
		if ( not view.empty() )
		{
			REG().patch<Scene::Camera>(
				*view.begin(),
				[ this ]( Scene::Camera & p_camera )
				{
					p_camera.screenHeight = _height;
					p_camera.screenWidth  = _width;
				}
			);
		}

		App::RENDERER().resize( _width, _height );
	}

} // namespace VTX::App::Action::Application
