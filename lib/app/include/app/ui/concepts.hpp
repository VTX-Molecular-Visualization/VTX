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
	  public:
		std::string				   name;
		std::optional<std::string> group = std::nullopt;
		std::optional<std::string> tip	 = std::nullopt;
		std::optional<std::string> icon	 = std::nullopt;
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

	/**
	 * @brief Main window requirements.
	 */
	template<typename MW>
	concept ConceptMainWindow = requires( MW p_mw, WidgetId p_id, DescAction p_action ) {
		{ p_mw.prepare() } -> std::same_as<void>;
		{ p_mw.build() } -> std::same_as<void>;
		{ p_mw.addMenuAction( p_id, p_action ) } -> std::same_as<void>;
		{ p_mw.addToolBarAction( p_id, p_action ) } -> std::same_as<void>;
	};

	// Convert ConceptMainWindow to a base class.
	/*
	class BaseMainWindow
	{
	  public:
		virtual void build()																	 = 0;
		virtual void addMenuAction( const WidgetId & p_menu, const DescAction & p_action )		 = 0;
		virtual void addToolBarAction( const WidgetId & p_toolbar, const DescAction & p_action ) = 0;
	};
	*/

} // namespace VTX::App::UI

#endif
