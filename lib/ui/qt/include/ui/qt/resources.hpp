#ifndef __VTX_UI_QT_RESOURCES__
#define __VTX_UI_QT_RESOURCES__

#include <QString>

namespace VTX::UI::QT::Resources
{
	// Stylesheets.
	constexpr std::string_view FILE_STYLESHEET		   = ":/stylesheet_ui.css";
	constexpr std::string_view FILE_STYLESHEET_WINDOWS = ":/stylesheet_ui_windows.css";
	constexpr std::string_view FILE_STYLESHEET_LINUX   = ":/stylesheet_ui_linux.css";
	constexpr std::string_view FILE_STYLESHEET_MACOS   = ":/stylesheet_ui_mac.css";

	// Sprites.
	constexpr std::string_view SPRITE_SPLASH = ":/sprite/splash.png";
	constexpr std::string_view SPRITE_LOGO	 = ":/sprite/logo.png";

	// Fonts.
	constexpr std::string_view FONT_MATERIAL_SYMBOLS = ":/font/MaterialSymbolsOutlined.ttf";
	constexpr std::string_view FONT_INTER			 = ":/font/Inter-VariableFont_opsz,wght.ttf";
	constexpr std::string_view FONT_OPEN_DYSLEXIC	 = ":/font/OpenDyslexic-Regular.otf";
} // namespace VTX::UI::QT::Resources

#endif
