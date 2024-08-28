#ifndef __VTX_UI_QT_RESOURCES__
#define __VTX_UI_QT_RESOURCES__

#include <QString>

namespace VTX::UI::QT::Resources
{
	// Stylesheets.
	constexpr std::string_view FILE_STYLESHEET		  = ":/stylesheet_ui.css";
	constexpr std::string_view FILE_STYLESHEET_WINOWS = ":/stylesheet_ui_windows.css";
	constexpr std::string_view FILE_STYLESHEET_LINUX  = ":/stylesheet_ui_linux.css";
	constexpr std::string_view FILE_STYLESHEET_MACOS  = ":/stylesheet_ui_mac.css";

	// Sprites.
	constexpr std::string_view SPRITE_SPLASH = ":/sprite/splash.png";
	constexpr std::string_view SPRITE_LOGO	 = ":/sprite/logo.png";
} // namespace VTX::UI::QT::Resources

#endif
