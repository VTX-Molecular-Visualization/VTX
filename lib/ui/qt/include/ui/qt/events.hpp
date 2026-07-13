#ifndef __VTX_UI_QT_EVENTS__
#define __VTX_UI_QT_EVENTS__

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

	struct ScriptSelected
	{
		FilePath path;
	};

} // namespace VTX::UI::QT::Events

#endif
