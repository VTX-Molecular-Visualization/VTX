#ifndef __VTX_UTIL_IMAGE__
#define __VTX_UTIL_IMAGE__

#include "types.hpp"

namespace VTX::Util::Image
{
	/**
	 * @brief Available image formats.
	 */
	enum struct E_FORMAT
	{
		PNG,
		JPG
	};

	/**
	 * @brief Write image to disk.
	 *
	 * @param p_path Path to write image to.
	 * @param p_format Image format.
	 * @param p_width Image width.
	 * @param p_height Image height.
	 * @param p_data Image data.
	 */
	void write(
		const FilePath &	p_path,
		const E_FORMAT		p_format,
		const size_t		p_width,
		const size_t		p_height,
		const uchar * const p_data
	);

} // namespace VTX::Util::Image

#endif
