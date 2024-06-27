#ifndef __VTX_UI_DESCRIPTORS__
#define __VTX_UI_DESCRIPTORS__

#include <app/core/input/keys.hpp>
#include <string>

namespace VTX::UI
{
	using WidgetId = std::string;

	struct BaseMainWindow
	{
	};

	struct MenuAction
	{
		WidgetId	idMenu;
		std::string name;
		std::string tip;
		std::string icon;
		// std::optional<App::Core::Input::Shortcut> shortcut;
		// callback or vtx action
	};

	struct ToolBarAction
	{
		WidgetId	idToolBar;
		std::string name;
	};

	struct DockPanel
	{
	};

	struct Modal
	{
	};

} // namespace VTX::UI

#endif
