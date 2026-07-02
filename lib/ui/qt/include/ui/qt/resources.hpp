#ifndef __VTX_UI_QT_RESOURCES__
#define __VTX_UI_QT_RESOURCES__

#include <QString>

namespace VTX::UI::QT::Resources
{

	// Sprites.
	constexpr std::string_view SPRITE_SPLASH	 = ":/sprite/splash.png";
	constexpr std::string_view SPRITE_LOGO		 = ":/sprite/logo.png";
	constexpr std::string_view SPRITE_SPLASH_GIF = ":/sprite/splash.gif";

	// Fonts.
	constexpr std::string_view FONT_MATERIAL_SYMBOLS = ":/font/MaterialSymbolsOutlined.ttf";
	constexpr std::string_view FONT_INTER			 = ":/font/Inter-VariableFont_opsz,wght.ttf";
	constexpr std::string_view FONT_LIBERATION_MONO	 = ":/font/LiberationMono-Regular.ttf";
	constexpr std::string_view FONT_CASCADIA_MONO	 = ":/font/CascadiaMono-VariableFont_wght.ttf";
	constexpr std::string_view FONT_CASCADIA_CODE	 = ":/font/CascadiaCode-VariableFont_wght.ttf";
	constexpr std::string_view FONT_OPEN_DYSLEXIC	 = ":/font/OpenDyslexic-Regular.otf";

	// Font families.
	constexpr std::string_view FONT_INTER_FAMILY		   = "Inter";
	constexpr std::string_view FONT_LIBERATION_MONO_FAMILY = "Liberation Mono";
	constexpr std::string_view FONT_CASCADIA_MONO_FAMILY   = "Cascadia Mono";
	constexpr std::string_view FONT_CASCADIA_CODE_FAMILY   = "Cascadia Code";
	constexpr std::string_view FONT_OPEN_DYSLEXIC_FAMILY   = "OpenDyslexic";

} // namespace VTX::UI::QT::Resources

#endif
