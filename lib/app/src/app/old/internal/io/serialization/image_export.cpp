#include "app/old/internal/io/serialization/image_export.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/render/spec.hpp"
#include "app/old/vtx_app.hpp"
#include <cctype>

namespace VTX::App::Old::Internal::IO::Serialization
{
	const ImageExport::Format ImageExport::getFormatFromExtension( const std::string & p_extension )
	{
		std::string lowCaseExtension = p_extension;
		std::transform( lowCaseExtension.begin(), lowCaseExtension.end(), lowCaseExtension.begin(), ::tolower );

		if ( lowCaseExtension == "png" )
			return Format::PNG;
		else if ( lowCaseExtension == "jpg" || lowCaseExtension == "jpeg" || lowCaseExtension == "jpe" )
			return Format::JPEG;
		else if ( lowCaseExtension == "bmp" || lowCaseExtension == "dib" || lowCaseExtension == "rle" )
			return Format::BMP;
		else
			return Format::UNKNOWN;
	}

	std::pair<int, int> ImageExport::getSnapshotWidthRange()
	{
		return { SNAPSHOT_MIN_SIZE, VTX_SPEC().glMaxTextureSize };
	}
	std::pair<int, int> ImageExport::getSnapshotHeightRange()
	{
		return { SNAPSHOT_MIN_SIZE, VTX_SPEC().glMaxTextureSize };
	}

	std::pair<int, int> ImageExport::getSize( const RESOLUTION & p_resolution )
	{
		if ( p_resolution == RESOLUTION::Free )
		{
			const App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();
			return std::pair<int, int>( camera.getScreenWidth(), camera.getScreenHeight() );
		}
		else
		{
			return RESOLUTION_SIZE[ int( p_resolution ) ];
		}
	}

} // namespace VTX::App::Old::Internal::IO::Serialization
