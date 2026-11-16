#ifndef VTX_UI_QT_RESOURCES_HPP
#define VTX_UI_QT_RESOURCES_HPP

#include <QString>

namespace VTX::UI::QT::Resources
{

	// Sprites.
	constexpr QUtf8StringView SPRITE_SPLASH { ":/sprite/splash.png" };
	constexpr QUtf8StringView SPRITE_LOGO { ":/sprite/logo.png" };
	constexpr QUtf8StringView SPRITE_SPLASH_GIF { ":/sprite/splash.gif" };

	// Fonts.
	constexpr QUtf8StringView FONT_MATERIAL_SYMBOLS { ":/font/MaterialSymbolsOutlined.ttf" };
	constexpr QUtf8StringView FONT_INTER { ":/font/Inter-VariableFont_opsz,wght.ttf" };
	constexpr QUtf8StringView FONT_LIBERATION_MONO { ":/font/LiberationMono-Regular.ttf" };
	constexpr QUtf8StringView FONT_CASCADIA_MONO { ":/font/CascadiaMono-VariableFont_wght.ttf" };
	constexpr QUtf8StringView FONT_CASCADIA_CODE { ":/font/CascadiaCode-VariableFont_wght.ttf" };
	constexpr QUtf8StringView FONT_OPEN_DYSLEXIC { ":/font/OpenDyslexic-Regular.otf" };

	// Font families.
	constexpr QUtf8StringView FONT_INTER_FAMILY { "Inter" };
	constexpr QUtf8StringView FONT_LIBERATION_MONO_FAMILY { "Liberation Mono" };
	constexpr QUtf8StringView FONT_CASCADIA_MONO_FAMILY { "Cascadia Mono" };
	constexpr QUtf8StringView FONT_CASCADIA_CODE_FAMILY { "Cascadia Code" };
	constexpr QUtf8StringView FONT_OPEN_DYSLEXIC_FAMILY { "OpenDyslexic" };

} // namespace VTX::UI::QT::Resources

#endif
