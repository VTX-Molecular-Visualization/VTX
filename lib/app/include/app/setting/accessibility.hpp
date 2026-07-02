#ifndef __VTX_APP_SETTING_ACCESSIBILITY__
#define __VTX_APP_SETTING_ACCESSIBILITY__

#include <util/types.hpp>

namespace VTX::App::Setting
{
	/**
	 * @brief Color modes.
	 */
	enum struct E_COLOR_ACCESSIBILITY_MODE : uint
	{
		STANDARD,
		HIGH_CONTRAST,
		COLORBLIND
	};

	/**
	 * @brief Default values.
	 */
	constexpr E_COLOR_ACCESSIBILITY_MODE COLOR_ACCESSIBILITY_MODE_DEFAULT = E_COLOR_ACCESSIBILITY_MODE::STANDARD;
	constexpr bool						 FORCE_DYSLEXIC_FONT_DEFAULT	  = false;

	/**
	 * @brief Accessibility settings.
	 */
	struct Accessibility
	{
		E_COLOR_ACCESSIBILITY_MODE colorMode		 = COLOR_ACCESSIBILITY_MODE_DEFAULT;
		bool					   forceDyslexicFont = FORCE_DYSLEXIC_FONT_DEFAULT;
	};
} // namespace VTX::App::Setting

#endif
