#ifndef __VTX_UI_QT_CONTEXTUAL_MENU__
#define __VTX_UI_QT_CONTEXTUAL_MENU__

#include "ui/core/define.hpp"
#include "ui/qt/widget/contextual_menu/contextual_menu_template.hpp"
#include <QPoint>
#include <map>

namespace VTX::UI::QT
{
	class ContextualMenu
	{
	  private:
		using MenusMap = std::map<Core::WidgetKey, Widget::ContextualMenu::BaseContextualMenu *>;

	  public:
		ContextualMenu();
		~ContextualMenu();

		void registerMenu( const Core::WidgetKey & p_menuKey, Widget::ContextualMenu::BaseContextualMenu * p_menu );
		Widget::ContextualMenu::BaseContextualMenu * const getMenu( const Core::WidgetKey & p_menuKey ) const;

		template<typename T,
				 typename = std::enable_if<std::is_base_of<Widget::ContextualMenu::BaseContextualMenu, T>::value>>
		T * const getMenu( const Core::WidgetKey & p_menuKey ) const
		{
			return dynamic_cast<T *>( getMenu( p_menuKey ) );
		}

		template<typename T>
		void pop( const Core::WidgetKey & p_menuKey, T * const p_target, const QPoint & p_worldPos ) const
		{
			Widget::ContextualMenu::BaseContextualMenu * const menu = getMenu( p_menuKey );

			if ( menu == nullptr )
				return;

			Widget::ContextualMenu::ContextualMenuTemplate<T> * const castedMenu
				= dynamic_cast<Widget::ContextualMenu::ContextualMenuTemplate<T> *>( menu );

			castedMenu->setTarget( p_target );

			castedMenu->popup( p_worldPos );
		}
		void pop( const Core::WidgetKey & p_menuKey, const QPoint & p_worldPos ) const
		{
			Widget::ContextualMenu::BaseContextualMenu * const menu = getMenu( p_menuKey );

			if ( menu != nullptr )
			{
				menu->popup( p_worldPos );
			}
		}

	  private:
		MenusMap _menusMap = MenusMap();
	};

} // namespace VTX::UI::QT

#endif
