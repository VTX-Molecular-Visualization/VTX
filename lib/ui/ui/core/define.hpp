#ifndef __VTX_UI_CORE_DEFINE__
#define __VTX_UI_CORE_DEFINE__

#include <string>

namespace VTX::UI::Core
{
	using WidgetKey = std::string;

	enum class Direction : int
	{
		Up,
		Down,
		Left,
		Right
	};
} // namespace VTX::UI::Core
#endif
