#include "app/action/io.hpp"
#include "app/action/scene.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/action/action_system.hpp"
#include "app/core/network/network_system.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/core/settings/settings_system.hpp"
#include "app/filesystem.hpp"
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

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
		FilePath filepath = _filename;
		NETWORK_SYSTEM().downloadFile(
			_url.str.data(),
			_filename.string(),
			[ filepath ]( const std::string & p_text )
			{ App::ACTION_SYSTEM().execute<App::Action::Scene::LoadSystem>( filepath, &p_text ); }
		);
	}

	LoadSettings::LoadSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void LoadSettings::execute() {}
	SaveSettings::SaveSettings() : _path( VTX::App::Filesystem::getSettingJsonFile() ) {}
	void SaveSettings::execute() {}
	void ReloadSettings::execute() {}
	void ResetSettings::execute() { SETTINGS_SYSTEM().reset(); }
	void SaveScene::execute() {}

	void OpenScene::execute() {}

	Snapshot::Snapshot(
		const FilePath				p_path,
		const Util::Image::E_FORMAT p_format,
		const size_t				p_width,
		const size_t				p_height
	) : _path( p_path ), _format( p_format ), _width( p_width ), _height( p_height )
	{
	}

	Snapshot::Snapshot()
	{
		const auto & camera = SCENE().getCamera();

		_path = Filesystem::getSnapshotsPath( std::to_string( Util::Chrono::getTimestamp() ) );
		// TODO: set value from settings.
		_format = Util::Image::E_FORMAT::PNG;
		_width	= camera.getScreenWidth();
		_height = camera.getScreenHeight();
	}

	void Snapshot::execute()
	{
		try
		{
			const auto & camera = SCENE().getCamera();

			std::vector<uchar> image;
			RENDERER_SYSTEM().snapshot( image, _width, _height, camera.getFov(), camera.getNear(), camera.getFar() );

			FilePath path = Util::Image::write( _path, _format, _width, _height, image.data() );

			VTX_INFO( "Image saved: {}", path.string() );
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Snapshot failed: {}", e.what() );
		}
	}
} // namespace VTX::App::Action::Io
