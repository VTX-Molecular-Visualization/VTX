#ifndef __VTX_APP_UI_CONCEPTS__
#define __VTX_APP_UI_CONCEPTS__

#include <functional>
#include <optional>
#include <string>

namespace VTX::App::UI
{
	using WidgetId = std::string;

	struct DescAction
	{
		std::string				   name;
		std::optional<std::string> group = std::nullopt;
		std::optional<std::string> tip	 = std::nullopt;
		std::optional<std::string> icon	 = std::nullopt;
		std::optional<std::string> shortcut;

		using TriggerFunction				   = std::function<void()>;
		std::optional<TriggerFunction> trigger = std::nullopt;
	};

	template<typename MW>
	concept ConceptMainWindow = requires( MW p_mw, WidgetId p_id, DescAction p_action ) {
		{ p_mw.build() } -> std::same_as<void>;
		{ p_mw.addMenuAction( p_id, p_action ) } -> std::same_as<void>;
		{ p_mw.addToolBarAction( p_id, p_action ) } -> std::same_as<void>;
	};

} // namespace VTX::App::UI

#endif
