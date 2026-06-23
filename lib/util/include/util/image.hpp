#ifndef __VTX_UTIL_IMAGE__
#define __VTX_UTIL_IMAGE__

#include "types.hpp"
#include <cstdint>
#include <optional>
#include <util/filesystem.hpp>
#include <vector>

namespace VTX::Util::Image
{
	/**
	 * @brief Available image formats.
	 */
	enum struct E_FORMAT
	{
		PNG,
		JPEG
	};

	/**
	 * @brief Write image to disk.
	 */
	FilePath write(
		const FilePath &		p_path,
		const E_FORMAT			p_format,
		const size_t			p_width,
		const size_t			p_height,
		const std::byte * const p_data
	);

	/**
	 * @brief Floating-point image representation.
	 */
	struct FloatImage
	{
		size_t			   width  = 0;
		size_t			   height = 0;
		std::vector<float> pixels;
	};

	struct ByteImage
	{
		size_t				 width	  = 0;
		size_t				 height	  = 0;
		uint8_t				 channels = 0;
		std::vector<uint8_t> pixels;
	};

	/**
	 * @brief Read an image with the requested number of 8-bit channels.
	 */
	ByteImage readByteImage( const FilePath &, const uint8_t p_channels = 4 );

	/**
	 * @brief Read an HDR image from disk.
	 */
	FloatImage readFloatImage( const FilePath & );

} // namespace VTX::Util::Image

#endif
