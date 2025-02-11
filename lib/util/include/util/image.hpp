#ifndef __VTX_UTIL_IMAGE__
#define __VTX_UTIL_IMAGE__

#include "types.hpp"
#include <optional>

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
	 *
	 * @param p_path Path to write image to.
	 * @param p_format Image format.
	 * @param p_width Image width.
	 * @param p_height Image height.
	 * @param p_data Image data.
	 * @param p_watermark Optional watermark path.
	 */
	FilePath write(
		const FilePath &			  p_path,
		const E_FORMAT				  p_format,
		const size_t				  p_width,
		const size_t				  p_height,
		const uchar * const			  p_data,
		const std::optional<FilePath> p_watermark = std::nullopt
	);

} // namespace VTX::Util::Image

#endif
