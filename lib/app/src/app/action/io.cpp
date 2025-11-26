#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/filesystem.hpp"
#include "app/network/network_manager.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::IO
{

	void Open::execute( const FilePath & p_path )
	{
		// TODO: check file format to redirect to the correct loader.
		ACTION().execute<Action::Scene::LoadSystem>( p_path );
	}

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id )
	{
		execute( p_id, p_id.str + VTX::Util::Url::rcsbPdbDownloadFileExtension() );
	}

	void DownloadSystem::execute( const char * p_systemId ) { execute( Util::Url::SystemId( p_systemId ) ); }

	void DownloadSystem::execute( VTX::Util::Url::SystemId p_id, FilePath p_path )
	{
		execute( VTX::Util::Url::UrlFull( p_id ), p_path );
	}

	void DownloadSystem::execute( VTX::Util::Url::UrlFull p_url, FilePath p_path )
	{
		FilePath filepath = p_path;
		NETWORK().downloadFile(
			p_url.str.data(),
			filepath.string(),
			[ filepath ]( const std::string & p_text )
			{ ACTION().execute<Action::Scene::LoadSystem>( filepath, &p_text ); }
		);
	}

	void Snapshot::execute()
	{
		execute(
			Filesystem::getSnapshotsDir() / std::to_string( Util::Chrono::getTimestamp() ),
			Util::Image::E_FORMAT::PNG,
			RENDERER().width(),
			RENDERER().height()
		);
	}

	void Snapshot::execute(
		const FilePath				p_path,
		const Util::Image::E_FORMAT p_format,
		const size_t				p_width,
		const size_t				p_height
	)
	{
		try
		{
			const auto &	   camera = ECS::getFirstComponent<App::Scene::Camera>();
			std::vector<uchar> image;

			RENDERER().snapshot( image, _width, _height, *camera.fov, *camera.near, *camera.far );
			FilePath path = Util::Image::write( _path, _format, _width, _height, image.data() );

			VTX_INFO( "Image saved: {}", path.string() );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Snapshot failed: {}", p_e.what() );
		}
	}
} // namespace VTX::App::Action::IO
