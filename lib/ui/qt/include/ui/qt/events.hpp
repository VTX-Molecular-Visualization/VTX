#ifndef VTX_UI_QT_EVENTS_HPP
#define VTX_UI_QT_EVENTS_HPP

#include <util/filesystem.hpp>

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

	struct TreeViewModeChanged
	{
		int viewMode;
	};

	struct SequenceResIdChanged
	{
		int residUsed;
	};

	struct ThemeChanged
	{
		int theme;
	};

	struct ScriptRun
	{
	};

} // namespace VTX::UI::QT::Events

#endif
