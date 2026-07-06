#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYEXR_IMPLEMENTATION

#include "util/image.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exiv2/exiv2.hpp>
#include <memory>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tinyexr.h>
#include <util/string.hpp>

namespace VTX::Util::Image
{
	namespace
	{
		FloatImage _readEXR( const FilePath & p_path )
		{
			float *		 pixels = nullptr;
			int			 width	= 0;
			int			 height = 0;
			const char * error	= nullptr;

			const int result = LoadEXR( &pixels, &width, &height, p_path.string().c_str(), &error );
			if ( result != TINYEXR_SUCCESS )
			{
				const std::string message = error != nullptr ? error : "unknown TinyEXR error";
				if ( error != nullptr )
				{
					FreeEXRErrorMessage( error );
				}
				throw std::runtime_error( "Unable to load EXR '" + p_path.string() + "': " + message );
			}

			FloatImage image;
			image.width	 = static_cast<size_t>( width );
			image.height = static_cast<size_t>( height );
			const std::unique_ptr<float, decltype( &std::free )> pixelsOwner( pixels, &std::free );
			image.pixels.assign( pixelsOwner.get(), pixelsOwner.get() + image.width * image.height * 4 );

			return image;
		}

		FloatImage _readRadianceHDR( const FilePath & p_path )
		{
			int			  width		 = 0;
			int			  height	 = 0;
			int			  components = 0;
			float * const pixels	 = stbi_loadf( p_path.string().c_str(), &width, &height, &components, 4 );
			if ( pixels == nullptr )
			{
				const char * const error = stbi_failure_reason();
				throw std::runtime_error(
					"Unable to load HDR '" + p_path.string()
					+ "': " + ( error != nullptr ? error : "unknown stb_image error" )
				);
			}

			FloatImage image;
			image.width	 = static_cast<size_t>( width );
			image.height = static_cast<size_t>( height );
			const std::unique_ptr<float, decltype( &stbi_image_free )> pixelsOwner( pixels, &stbi_image_free );
			image.pixels.assign( pixelsOwner.get(), pixelsOwner.get() + image.width * image.height * 4 );

			return image;
		}

		ByteImage _readByteImage( const FilePath & p_path, const uint8_t p_channels )
		{
			if ( p_channels == 0 || p_channels > 4 )
			{
				throw std::runtime_error( "Invalid requested channel count" );
			}

			int		  width			 = 0;
			int		  height		 = 0;
			int		  sourceChannels = 0;
			stbi_uc * pixels
				= stbi_load( p_path.string().c_str(), &width, &height, &sourceChannels, int( p_channels ) );
			if ( pixels == nullptr )
			{
				const char * const error = stbi_failure_reason();
				throw std::runtime_error(
					"Unable to load image '" + p_path.string()
					+ "': " + ( error != nullptr ? error : "unknown stb_image error" )
				);
			}

			ByteImage image;
			image.width	   = static_cast<size_t>( width );
			image.height   = static_cast<size_t>( height );
			image.channels = p_channels;
			const std::unique_ptr<stbi_uc, decltype( &stbi_image_free )> pixelsOwner( pixels, &stbi_image_free );
			image.pixels.assign(
				pixelsOwner.get(), pixelsOwner.get() + image.width * image.height * size_t( image.channels )
			);

			return image;
		}
	} // namespace

	FilePath write(
		const FilePath &		p_path,
		const E_FORMAT			p_format,
		const size_t			p_width,
		const size_t			p_height,
		const std::byte * const p_data
	)
	{
		// Remove extension from path.
		FilePath path = p_path.parent_path() / p_path.stem();

		// Create directories if needed.
		std::filesystem::create_directories( path.parent_path() );

		stbi_flip_vertically_on_write( true );

		// Save image.
		switch ( p_format )
		{
		case E_FORMAT::PNG:
			path += ".png";
			stbi_write_png_compression_level = 0;
			stbi_write_png( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 0 );
			break;
		case E_FORMAT::JPEG:
			path += ".jpg";
			stbi_write_jpg( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 100 );
			break;

		default: assert( 1 ); ;
		}

		return path;
	}

	void writeSoftwareMetadata( const FilePath & p_path, const std::string_view p_software )
	{
		if ( p_software.empty() )
		{
			return;
		}

		auto image = Exiv2::ImageFactory::open( p_path.string() );
		if ( image == nullptr )
		{
			throw std::runtime_error( "Unable to open image '" + p_path.string() + "' for metadata writing" );
		}

		image->readMetadata();

		Exiv2::ExifData & exifData		  = image->exifData();
		exifData[ "Exif.Image.Software" ] = std::string( p_software );
		image->setExifData( exifData );

		Exiv2::XmpData & xmpData		 = image->xmpData();
		xmpData[ "Xmp.xmp.CreatorTool" ] = std::string( p_software );
		image->setXmpData( xmpData );

		image->writeMetadata();
	}

	ByteImage readByteImage( const FilePath & p_path, const uint8_t p_channels )
	{ return _readByteImage( p_path, p_channels ); }

	FloatImage readFloatImage( const FilePath & p_path )
	{
		const std::string extension = Util::String::toLower( p_path.extension().string() );

		if ( extension == ".exr" )
		{
			return _readEXR( p_path );
		}
		if ( extension == ".hdr" )
		{
			return _readRadianceHDR( p_path );
		}

		throw std::runtime_error( "Unsupported HDR image format: '" + p_path.string() + "'" );
	}

} // namespace VTX::Util::Image
