#include "app/action/io.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/scene.hpp"
#include "app/filesystem.hpp"
#include "app/network/network_manager.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include <renderer/facade.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::IO
{

	void Open::execute( const FilePath & p_path )
	{
		// TODO: check file format to redirect to the correct loader.
		ACTION().execute<Action::Scene::LoadSystem>( p_path );
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
		NETWORK().downloadFile(
			_url.str.data(),
			_filename.string(),
			[ filepath ]( const std::string & p_text )
			{ ACTION().execute<Action::Scene::LoadSystem>( filepath, &p_text ); }
		);
	}

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
		_path	= Filesystem::getSnapshotsDir() / std::to_string( Util::Chrono::getTimestamp() );
		_format = Util::Image::E_FORMAT::PNG;
		_width	= RENDERER().getWidth();
		_height = RENDERER().getHeight();
	}

	void Snapshot::execute()
	{
		try
		{
			auto view = REG().view<App::Scene::Camera>();
			if ( not view.empty() )
			{
				const auto &	   camera = view.get<App::Scene::Camera>( *view.begin() );
				std::vector<uchar> image;

				RENDERER().snapshot( image, _width, _height, *camera.fov, *camera.near, *camera.far );
				FilePath path = Util::Image::write( _path, _format, _width, _height, image.data() );

				VTX_INFO( "Image saved: {}", path.string() );
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Snapshot failed: {}", p_e.what() );
		}
	}
} // namespace VTX::App::Action::IO
