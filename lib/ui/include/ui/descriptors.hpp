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

	struct Action
	{
		std::string				   name;
		std::optional<std::string> tip	= std::nullopt;
		std::optional<std::string> icon = std::nullopt;
		// callback or vtx action
		// std::optional<App::Core::Input::Shortcut> shortcut;
	};

	struct MenuAction
	{
		WidgetId idMenu;
		Action	 action;
	};

	struct ToolBarAction
	{
		WidgetId idToolBar;
		Action	 action;
	};

	struct DockPanel
	{
	};

	struct Modal
	{
	};

} // namespace VTX::UI

#endif
