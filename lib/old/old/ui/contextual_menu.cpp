#include "contextual_menu.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/contextual_menu/contextual_menu_atom.hpp"
#include "ui/widget/contextual_menu/contextual_menu_chain.hpp"
#include "ui/widget/contextual_menu/contextual_menu_label.hpp"
#include "ui/widget/contextual_menu/contextual_menu_molecule.hpp"
#include "ui/widget/contextual_menu/contextual_menu_path.hpp"
#include "ui/widget/contextual_menu/contextual_menu_render.hpp"
#include "ui/widget/contextual_menu/contextual_menu_residue.hpp"
#include "ui/widget/contextual_menu/contextual_menu_scene.hpp"
#include "ui/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/widget/contextual_menu/contextual_menu_viewpoint.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI
{
	const ContextualMenu & ContextualMenu::_getInstance() { return VTXApp::get().getMainWindow().getContextualMenu(); }

	ContextualMenu::ContextualMenu() { _buildMenus(); }

	ContextualMenu ::~ContextualMenu()
	{
		while ( _menus.size() > 0 )
		{
			QMenu * const item = _menus.back();
			_menus.pop_back();
			delete item;
		}
	}

	void ContextualMenu::_buildMenus()
	{
		_menus.resize( int( Menu::COUNT ) );

		QWidget * const parent = &VTXApp::get().getMainWindow();

		_menus[ int( Menu::Molecule ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuMolecule>(
				parent, "contextualMenuMolecule" );
		_menus[ int( Menu::Chain ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuChain>(
				parent, "contextualMenuChain" );
		_menus[ int( Menu::Residue ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuResidue>(
				parent, "contextualMenuResidue" );
		_menus[ int( Menu::Atom ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuAtom>(
				parent, "contextualMenuAtom" );
		_menus[ int( Menu::Selection ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuSelection>(
				parent, "contextualMenuSelection" );
		_menus[ int( Menu::Scene ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuScene>(
				parent, "contextualMenuScene" );
		_menus[ int( Menu::Render ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuRender>(
				parent, "contextualMenuRender" );
		_menus[ int( Menu::Path ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuPath>(
				parent, "ContextualMenuPath" );
		_menus[ int( Menu::Viewpoint ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuViewpoint>(
				parent, "ContextualMenuViewpoint" );
		_menus[ int( Menu::Label ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuLabel>(
				parent, "ContextualMenuLabel" );

		for ( QMenu * const menu : _menus )
		{
			menu->installEventFilter( parent );
		}
	}

} // namespace VTX::UI
