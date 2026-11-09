#ifndef VTX_APP_UI_CONCEPTS_HPP
#define VTX_APP_UI_CONCEPTS_HPP

#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace VTX::App::UI
{
	using WidgetId = std::string;

	/**
	 * @brief Desribes an UI action that can be added to a menu or toolbar.
	 */
	struct DescAction
	{
		/**
		 * @brief Icon can be a sprite path (string), uint (codepoint).
		 */
		using Icon = std::variant<std::string, int>;

		std::string				   key;
		std::string				   name;
		std::optional<std::string> tip		 = std::nullopt;
		std::optional<Icon>		   icon		 = std::nullopt;
		std::optional<std::string> shortcut	 = std::nullopt;
		std::optional<std::string> group	 = std::nullopt;
		bool					   checkable = false;
	};
} // namespace VTX::App::UI

#endif
