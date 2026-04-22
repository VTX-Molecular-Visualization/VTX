#ifndef __VTX_UI_QT_EVENTS__
#define __VTX_UI_QT_EVENTS__

namespace VTX::UI::QT::Events
{

	struct KeyboardLayoutChanged
	{
		int layout;
	};

	struct SelectionLocked
	{
		bool locked;
	};

	struct SelectionGranularityChanged
	{
		int granularity;
	};

	struct ThemeChanged
	{
		int theme;
	};

} // namespace VTX::UI::QT::Events

#endif
