#ifndef __VTX_UTIL_RESOLUTION__
#define __VTX_UTIL_RESOLUTION__

#include <cstddef>
#include <string_view>

namespace VTX::Util
{
	/**
	 * @brief Describes a screen resolution.
	 */
	struct Resolution
	{
		std::string_view name;
		size_t			 width;
		size_t			 height;
	};

	/**
	 * @brief Predefined screen resolutions.
	 */
	namespace ResolutionPreset
	{
		inline constexpr Resolution SVGA	   = { "SVGA", 800, 600 };
		inline constexpr Resolution XGA		   = { "XGA", 1024, 768 };
		inline constexpr Resolution HD		   = { "HD", 1280, 720 };
		inline constexpr Resolution WXGA	   = { "WXGA", 1280, 800 };
		inline constexpr Resolution SXGA	   = { "SXGA", 1280, 1024 };
		inline constexpr Resolution WXGA_PLUS  = { "WXGA+", 1440, 900 };
		inline constexpr Resolution HD_PLUS	   = { "HD+", 1600, 900 };
		inline constexpr Resolution WSXGA_PLUS = { "WSXGA+", 1680, 1050 };
		inline constexpr Resolution FULL_HD	   = { "Full HD", 1920, 1080 };
		inline constexpr Resolution WUXGA	   = { "WUXGA", 1920, 1200 };
		inline constexpr Resolution QHD		   = { "QHD", 2560, 1440 };
		inline constexpr Resolution WQHD	   = { "WQHD", 2560, 1440 };
		inline constexpr Resolution WQXGA	   = { "WQXGA", 2560, 1600 };
		inline constexpr Resolution UHD_4K	   = { "4K UHD", 3840, 2160 };
		inline constexpr Resolution UHD_8K	   = { "8K UHD", 7680, 4320 };
		inline constexpr Resolution UHD_16K	   = { "16K UHD", 15360, 8640 };
	} // namespace ResolutionPreset
} // namespace VTX::Util

#endif
