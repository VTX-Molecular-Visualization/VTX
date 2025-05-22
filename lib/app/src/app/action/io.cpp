#include "app/action/io.hpp"
#include "app/action/scene.hpp"
#include "app/core/network/network_system.hpp"
#include "app/filesystem.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/settings/settings_system.hpp"

#include <util/filesystem.hpp>


namespace VTX::App::Action::Io
{

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
			Action::Io::OpenScene openSceneAction { scenePath };
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

	void DownloadSystem::execute()
	{
		std::string	   data;
		const FilePath cachePath = Filesystem::getCachePath( _filename );

		NETWORK_SYSTEM().downloadFile( _url, _filename.string(), &data, true );
		App::ACTION_SYSTEM().execute<App::Action::Scene::LoadSystem>( _filename, &data );
	}

	

	LoadSettings::LoadSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void LoadSettings::execute() {}
	SaveSettings::SaveSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void SaveSettings::execute() {}
	void ReloadSettings::execute() {}
	void ResetSettings::execute() { SETTINGS_SYSTEM().reset(); }
	void SaveScene::execute() {}

	void OpenScene::execute() {}
}