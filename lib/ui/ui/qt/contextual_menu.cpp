#include "contextual_menu.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"

namespace VTX::UI::QT
{
	ContextualMenu::ContextualMenu() {}

	ContextualMenu ::~ContextualMenu()
	{
		for ( auto pair : _menusMap )
		{
			delete pair.second;
		}

		_menusMap.clear();
	}

	void ContextualMenu::registerMenu( const Core::WidgetKey &						p_menuKey,
									   Widget::ContextualMenu::BaseContextualMenu * p_menu )
	{
		QT::MainWindow * const mainWindow = &QT_APP()->getMainWindow();
		p_menu->setParent( mainWindow );

		_menusMap[ p_menuKey ] = p_menu;

		p_menu->installEventFilter( mainWindow );
	}

	Widget::ContextualMenu::BaseContextualMenu * const ContextualMenu::getMenu(
		const Core::WidgetKey & p_menuKey ) const
	{
		const MenusMap::const_iterator it = _menusMap.find( p_menuKey );

		if ( it == _menusMap.end() )
		{
			return nullptr;
		}

		return it->second;
	}

	// void ContextualMenu::_buildMenus()
	//{
	//_menus.resize( int( Menu::COUNT ) );

	// QWidget * const parent = &VTXApp::get().getMainWindow();

	//_menus[ int( Menu::Molecule ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuMolecule>(
	//		parent, "contextualMenuMolecule" );
	//_menus[ int( Menu::Chain ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuChain>(
	//		parent, "contextualMenuChain" );
	//_menus[ int( Menu::Residue ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuResidue>(
	//		parent, "contextualMenuResidue" );
	//_menus[ int( Menu::Atom ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuAtom>(
	//		parent, "contextualMenuAtom" );
	//_menus[ int( Menu::Selection ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuSelection>(
	//		parent, "contextualMenuSelection" );
	//_menus[ int( Menu::Scene ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuScene>(
	//		parent, "contextualMenuScene" );
	//_menus[ int( Menu::Render ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuRender>(
	//		parent, "contextualMenuRender" );
	//_menus[ int( Menu::Path ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuPath>(
	//		parent, "ContextualMenuPath" );
	//_menus[ int( Menu::Viewpoint ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuViewpoint>(
	//		parent, "ContextualMenuViewpoint" );
	//_menus[ int( Menu::Label ) ]
	//	= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuLabel>(
	//		parent, "ContextualMenuLabel" );

	// for ( QMenu * const menu : _menus )
	//{
	//	menu->installEventFilter( parent );
	// }
	//}

} // namespace VTX::UI::QT
