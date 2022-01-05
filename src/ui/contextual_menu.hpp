#ifndef __VTX_UI_CONTEXTUAL_MENU__
#define __VTX_UI_CONTEXTUAL_MENU__

#include "ui/widget/contextual_menu/contextual_menu_template.hpp"
#include <QMenu>
#include <QPoint>
#include <type_traits>
#include <vector>

namespace VTX::UI
{
	class ContextualMenu
	{
	  public:
		enum class Menu
		{
			Molecule,
			Chain,
			Residue,
			Atom,
			Selection,
			Scene,
			Render,
			Path,
			Viewpoint,

			COUNT
		};

		ContextualMenu();
		~ContextualMenu();

		template<typename P>
		static void pop( const Menu & p_menu, P * const p_target, const QPoint & p_worldPos )
		{
			_getInstance().displayMenu<P>( p_menu, p_target, p_worldPos );
		}
		static void pop( const Menu & p_menu, const QPoint & p_worldPos )
		{
			_getInstance().displayMenu( p_menu, p_worldPos );
		}

		template<typename T, typename = std::enable_if<std::is_base_of<QMenu, T>::value>>
		static T * const getMenu( const Menu & p_menu )
		{
			return dynamic_cast<T * const>( _getInstance()._menus[ int( p_menu ) ] );
		}

		template<typename P>
		void displayMenu( const Menu & p_menu, P * const p_target, const QPoint & p_worldPos ) const
		{
			Widget::ContextualMenu::ContextualMenuTemplate<P> * const menu
				= dynamic_cast<Widget::ContextualMenu::ContextualMenuTemplate<P> *>( _menus[ int( p_menu ) ] );
			menu->setTarget( p_target );
			_menus[ int( p_menu ) ]->popup( p_worldPos );
		}
		void displayMenu( const Menu & p_menu, const QPoint & p_worldPos ) const
		{
			_menus[ int( p_menu ) ]->popup( p_worldPos );
		}

	  private:
		std::vector<QMenu *> _menus = std::vector<QMenu *>();

		static const ContextualMenu & _getInstance();

		void _buildMenus();
	};

} // namespace VTX::UI

#endif
