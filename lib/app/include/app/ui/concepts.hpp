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
		using Icon = std::variant<std::string, int>;

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
		using Callable					= std::function<void()>;
		std::optional<Callable> trigger = std::nullopt;

		/**
		 * @brief Override this function to connect the action to the application callbacks.
		 * Used to update the UI when the application state changes, or set default values.
		 */
		virtual void connect() const {}
	};

	template<typename A>
	concept ConceptAction = std::is_base_of_v<DescAction, A>;

} // namespace VTX::App::UI

#endif
