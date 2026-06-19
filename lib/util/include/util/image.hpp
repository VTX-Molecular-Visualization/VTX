#ifndef __VTX_UTIL_IMAGE__
#define __VTX_UTIL_IMAGE__

#include "types.hpp"
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
	 * @brief EXR image representation.
	 */
	struct FloatImage
	{
		size_t			   width  = 0;
		size_t			   height = 0;
		std::vector<float> pixels;
	};

	/**
	 * @brief Read EXR image from disk.
	 */
	FloatImage readEXR( const FilePath & );

} // namespace VTX::Util::Image

#endif
