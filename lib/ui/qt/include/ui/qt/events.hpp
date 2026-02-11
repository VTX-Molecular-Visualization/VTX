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

} // namespace VTX::UI::QT::Events

#endif
