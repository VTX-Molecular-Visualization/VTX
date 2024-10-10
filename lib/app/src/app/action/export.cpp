#include "app/action/export.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/filesystem.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::Export
{
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

} // namespace VTX::App::Action::Export
