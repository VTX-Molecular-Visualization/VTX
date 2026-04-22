#ifndef __VTX_UI_QT_ACTION_REGISTRY__
#define __VTX_UI_QT_ACTION_REGISTRY__

#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <app/ui/concepts.hpp>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

class QWidget;

namespace VTX::UI::QT
{
	/**
	 * @brief Handle qAction described by App::UI::DescAction.
	 */
	class ActionRegistry
	{
	  public:
		using ActionValue  = std::variant<bool, int, double, std::string>;
		using ActionParams = std::unordered_map<std::string, ActionValue>;

		struct ActionContext
		{
			std::string_view actionId;
			ActionParams	 params;

			template<typename T>
			std::optional<T> param( const std::string_view p_key ) const
			{
				const auto it = params.find( std::string { p_key } );
				if ( it == params.end() )
				{
					return std::nullopt;
				}

				if ( const T * const value = std::get_if<T>( &it->second ) )
				{
					return *value;
				}

				return std::nullopt;
			}
		};

		using ActionHandler = std::function<void( const ActionContext & )>;

		/**
		 * @brief Get the key for an action description.
		 */
		static std::string key( const App::UI::DescAction & );

		/**
		 * @brief Register an action.
		 */
		QAction * registerAction( const App::UI::DescAction &, const ActionHandler = {} );

		/**
		 * @brief Get a registered action.
		 */
		QAction * getAction( const std::string_view ) const;

		/**
		 * @brief Trigger by key.
		 */
		void trigger( const std::string_view, ActionParams = {} ) const;

		/**
		 * @brief Get the checked state.
		 */
		bool isChecked( const std::string_view ) const;

		/**
		 * @brief Set the checked state.
		 */
		void setChecked( const std::string_view, const bool ) const;

		/**
		 * @brief Attach all action shortcuts to a widget.
		 */
		void installShortcuts( QWidget & ) const;

		/**
		 * @brief Add an action to a menu.
		 */
		QAction * addMenuAction( QMenu &, const std::string_view );

		/**
		 * @brief Add an action to a toolbar.
		 */
		QAction * addToolBarAction( QToolBar &, const std::string_view );

	  private:
		/**
		 * @brief Map of registered action handlers.
		 */
		std::unordered_map<std::string, ActionHandler> _handlers;

		/**
		 * @brief Create a QAction from an action description and register its handler.
		 */
		QAction * _createAction( const App::UI::DescAction &, const std::string_view );

		/**
		 * @brief Bind the trigger signal to the registry method.
		 */
		void _bindTrigger( QAction &, const std::string_view );
	};
} // namespace VTX::UI::QT

#endif
