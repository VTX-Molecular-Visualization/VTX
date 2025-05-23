#include "app/action/io.hpp"
#include "app/action/scene.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/network/network_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/filesystem.hpp"
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

	DownloadSystem::DownloadSystem( VTX::Util::Url::SystemId p_id ) :
		_url( std::move( p_id ) ), _filename( p_id.str + VTX::Util::Url::rcsbPdbDownloadFileExtension() )
	{
	}

	DownloadSystem::DownloadSystem( const char * p_systemId ) : DownloadSystem( Util::Url::SystemId( p_systemId ) ) {}

	DownloadSystem::DownloadSystem( VTX::Util::Url::SystemId p_id, FilePath p_path ) :
		DownloadSystem( VTX::Util::Url::UrlFull( p_id ), p_path )
	{
	}

	DownloadSystem::DownloadSystem( VTX::Util::Url::UrlFull p_url, FilePath p_path ) :
		_url( std::move( p_url ) ), _filename( p_path )
	{
	}

	void DownloadSystem::execute()
	{
		std::string	   data;
		const FilePath cachePath = Filesystem::getCachePath( _filename );

		NETWORK_SYSTEM().downloadFile( _url.str.data(), _filename.string(), &data, true );
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
} // namespace VTX::App::Action::Io
