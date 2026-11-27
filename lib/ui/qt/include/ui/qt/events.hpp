#pragma once

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

	struct ScriptSave
	{
	};

	struct ScriptCopied
	{
		FilePath path;
	};

} // namespace VTX::UI::QT::Events
