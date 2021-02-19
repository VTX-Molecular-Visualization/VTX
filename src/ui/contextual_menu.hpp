#ifndef __VTX_UI_CONTEXTUAL_MENU__
#define __VTX_UI_CONTEXTUAL_MENU__

#ifdef _MSC_VER
#pragma once
#endif

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
			InstantiatedRepresentation,

			COUNT
		};

		ContextualMenu();
		~ContextualMenu();

		template<typename T, typename = std::enable_if<std::is_base_of<QMenu, T>::value>>
		const T * const getMenu( const Menu & p_menu ) const
		{
			return dynamic_cast<T *>( _menus[ int( p_menu ) ] );
		}

		template<typename T,
				 typename P,
				 typename
				 = std::enable_if<std::is_base_of<Widget::ContextualMenu::ContextualMenuTemplate<P>, T>::value>>
		void displayMenu( const Menu & p_menu, P * const p_target, const QPoint & p_worldPos ) const
		{
			T * const menu = dynamic_cast<T *>( _menus[ int( p_menu ) ] );
			menu->setTarget( p_target );
			_menus[ int( p_menu ) ]->popup( p_worldPos );
		}

	  private:
		std::vector<QMenu *> _menus = std::vector<QMenu *>();

		void _buildMenus();
	};

} // namespace VTX::UI

#endif
