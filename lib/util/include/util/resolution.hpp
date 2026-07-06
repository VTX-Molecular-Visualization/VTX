#ifndef __VTX_UTIL_RESOLUTION__
#define __VTX_UTIL_RESOLUTION__

#include <cstddef>
#include <string>

namespace VTX::Util
{
	/**
	 * @brief Describes a screen resolution.
	 */
	struct Resolution
	{
		std::string name;
		size_t		width;
		size_t		height;
	};

	/**
	 * @brief Predefined screen resolutions.
	 */
	namespace ResolutionPreset
	{
		inline const Resolution SVGA	   = { "SVGA", 800, 600 };
		inline const Resolution XGA		   = { "XGA", 1024, 768 };
		inline const Resolution HD		   = { "HD", 1280, 720 };
		inline const Resolution WXGA	   = { "WXGA", 1280, 800 };
		inline const Resolution SXGA	   = { "SXGA", 1280, 1024 };
		inline const Resolution WXGA_PLUS  = { "WXGA+", 1440, 900 };
		inline const Resolution HD_PLUS	   = { "HD+", 1600, 900 };
		inline const Resolution WSXGA_PLUS = { "WSXGA+", 1680, 1050 };
		inline const Resolution FULL_HD	   = { "Full HD", 1920, 1080 };
		inline const Resolution WUXGA	   = { "WUXGA", 1920, 1200 };
		inline const Resolution QHD		   = { "QHD", 2560, 1440 };
		inline const Resolution WQHD	   = { "WQHD", 2560, 1440 };
		inline const Resolution WQXGA	   = { "WQXGA", 2560, 1600 };
		inline const Resolution UHD_4K	   = { "4K UHD", 3840, 2160 };
		inline const Resolution UHD_8K	   = { "8K UHD", 7680, 4320 };
		inline const Resolution UHD_16K	   = { "16K UHD", 15360, 8640 };
	} // namespace ResolutionPreset
} // namespace VTX::Util

#endif
