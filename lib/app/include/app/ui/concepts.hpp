#ifndef __VTX_APP_UI_CONCEPTS__
#define __VTX_APP_UI_CONCEPTS__

#include <functional>
#include <optional>
#include <string>

namespace VTX::App::UI
{
	using WidgetId = std::string;

	/**
	 * @brief Desribes an UI action that can be added to a menu or toolbar.
	 */
	struct DescAction
	{
		/**
		 * @brief Icon can be a sprite path (string) or an integer (icon).
		 */
		using Icon	   = std::variant<std::string, int>;
		using Callable = std::function<void()>;

	  public:
		std::string				   key;
		std::string				   name;
		std::optional<std::string> group = std::nullopt;
		std::optional<std::string> tip	 = std::nullopt;
		std::optional<Icon>		   icon	 = std::nullopt;
		std::optional<std::string> shortcut;

		/**
		 * @brief Trigger function is called when button is clicked.
		 */
		std::optional<Callable> trigger = std::nullopt;
	};

	template<typename A>
	concept ConceptAction = std::is_base_of_v<DescAction, A>;

} // namespace VTX::App::UI

#endif
