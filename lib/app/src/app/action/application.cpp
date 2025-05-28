#include "app/action/application.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/filesystem.hpp"
#include <app/component/render/camera.hpp>

namespace VTX::App::Action::Application
{
	void NewScene::execute() { SCENE().reset(); }




	void Quit::execute()
	{
		APP::onEndOfFrameOneShot += []() { APP::stop(); };
	}

	void Resize::execute()
	{
		App::SCENE().getCamera().setScreenSize( _width, _height );
		App::RENDERER_SYSTEM().resize( _width, _height );
	}

} // namespace VTX::App::Action::Application
