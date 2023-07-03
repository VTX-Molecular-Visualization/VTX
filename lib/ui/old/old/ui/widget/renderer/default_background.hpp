#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_DEFAULT_BACKGROUND__
#define __VTX_UI_WIDGET_RENDER_EFFECT_DEFAULT_BACKGROUND__

#include <QIcon>
#include <string>
#include <util/constants.hpp>
#include <vector>

namespace VTX::UI::Widget::Renderer
{
	enum class DEFAULT_BACKGROUND : int
	{
		BLACK,
		WHITE,

		COUNT,
	};

	inline static const std::vector<std::string> DEFAULT_BACKGROUND_STR = { "Black", "White" };
	inline static const std::vector<std::string> DEFAULT_BACKGROUND_ICON_PATH
		= { ":/sprite/background_black_icon.png", ":/sprite/background_white_icon.png" };
	inline static const std::vector<Util::Color::Rgba> DEFAULT_BACKGROUND_COLORS = { COLOR_BLACK, COLOR_WHITE };

} // namespace VTX::UI::Widget::Renderer
#endif
