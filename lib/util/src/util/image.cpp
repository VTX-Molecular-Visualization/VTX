#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "util/image.hpp"
#include <stb_image_write.h>

namespace VTX::Util::Image
{

	FilePath write(
		const FilePath &	p_path,
		const E_FORMAT		p_format,
		const size_t		p_width,
		const size_t		p_height,
		const uchar * const p_data
	)
	{
		// Remove extension from path.
		FilePath path = p_path.parent_path() / p_path.stem();

		stbi_flip_vertically_on_write( true );

		switch ( p_format )
		{
		case E_FORMAT::PNG:
			path += ".png";
			stbi_write_png_compression_level = 0;
			stbi_write_png( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 0 );
			break;
		case E_FORMAT::JPG:
			path += ".jpg";
			stbi_write_jpg( path.string().c_str(), int( p_width ), int( p_height ), 4, p_data, 100 );
			break;

		default: assert( 1 ); ;
		}

		return path;
	}
} // namespace VTX::Util::Image
