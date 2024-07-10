#ifndef __VTX_UI_DESCRIPTORS__
#define __VTX_UI_DESCRIPTORS__

#include <app/core/input/keys.hpp>
#include <string>

namespace VTX::UI
{

	struct Action
	{
		std::string				   name;
		std::optional<std::string> tip	= std::nullopt;
		std::optional<std::string> icon = std::nullopt;

		// Or a custom function in App::Action.

		using TriggerFunction				   = std::function<void()>;
		std::optional<TriggerFunction> trigger = std::nullopt;

		// std::optional<App::Core::Input::Shortcut> shortcut;

		void setTrigger( const TriggerFunction & p_trigger ) { trigger = p_trigger; }
	};

	using WidgetId = std::string;

	/*
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
	*/

	struct DockPanel
	{
	};

	struct Modal
	{
	};

} // namespace VTX::UI

#endif
