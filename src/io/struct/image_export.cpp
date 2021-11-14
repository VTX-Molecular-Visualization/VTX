#include "image_export.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"
#include <cctype>

namespace VTX::IO::Struct
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

	std::pair<int, int> ImageExport::getSize( const RESOLUTION & p_resolution )
	{
		if ( p_resolution == RESOLUTION::Free )
		{
			const Object3D::Camera & camera = VTXApp::get().getScene().getCamera();
			return std::pair<int, int>( camera.getScreenWidth(), camera.getScreenHeight() );
		}
		else
		{
			return RESOLUTION_SIZE[ int( p_resolution ) ];
		}
	}

} // namespace VTX::IO::Struct
