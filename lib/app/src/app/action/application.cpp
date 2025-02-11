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

	void OpenScene::execute() {}

	void SaveScene::execute() {}
	void ClearScene::execute() { SCENE().reset(); }

	LoadSettings::LoadSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void LoadSettings::execute() {}
	SaveSettings::SaveSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void SaveSettings::execute() {}
	void ReloadSettings::execute() {}
	void ResetSettings::execute() { SETTINGS_SYSTEM().reset(); }

	void Open::execute()
	{
		FilePath scenePath = "";
		for ( const FilePath & path : _paths )
		{
			if ( path.extension() == "vtx" )
			{
				scenePath = path;
				break;
			}
		}

		if ( not scenePath.empty() )
		{
			Action::Application::OpenScene openSceneAction = Action::Application::OpenScene( scenePath );
			openSceneAction.execute();
		}
		else
		{
			for ( const FilePath & systemPath : _paths )
			{
				Action::Scene::LoadSystem loadSystemAction = Action::Scene::LoadSystem( systemPath );
				loadSystemAction.execute();
			}
		}
	}

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
