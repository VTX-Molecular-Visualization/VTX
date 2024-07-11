#ifndef __VTX_UI_DESCRIPTORS__
#define __VTX_UI_DESCRIPTORS__

#include <functional>
#include <string>

namespace VTX::UI
{

	struct Action
	{
		std::string				   name;
		std::optional<std::string> group = std::nullopt;
		std::optional<std::string> tip	 = std::nullopt;
		std::optional<std::string> icon	 = std::nullopt;
		std::optional<std::string> shortcut;

		using TriggerFunction				   = std::function<void()>;
		std::optional<TriggerFunction> trigger = std::nullopt;
	};

	using WidgetId = std::string;

} // namespace VTX::UI

#endif
