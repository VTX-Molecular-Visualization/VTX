#include "contextual_menu.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/contextual_menu/contextual_menu_atom.hpp"
#include "ui/widget/contextual_menu/contextual_menu_chain.hpp"
#include "ui/widget/contextual_menu/contextual_menu_molecule.hpp"
#include "ui/widget/contextual_menu/contextual_menu_residue.hpp"
#include "ui/widget/contextual_menu/contextual_menu_scene.hpp"
#include "ui/widget/contextual_menu/contextual_menu_render.hpp"
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

		_menus[ int( Menu::Molecule ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuMolecule>(
				nullptr, "contextualMenuMolecule" );
		_menus[ int( Menu::Chain ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuChain>(
				nullptr, "contextualMenuChain" );
		_menus[ int( Menu::Residue ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuResidue>(
				nullptr, "contextualMenuResidue" );
		_menus[ int( Menu::Atom ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuAtom>(
				nullptr, "contextualMenuAtom" );
		_menus[ int( Menu::Selection ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuSelection>(
				nullptr, "contextualMenuSelection" );
		_menus[ int( Menu::Scene ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuScene>(
				nullptr, "contextualMenuScene" );
		_menus[ int( Menu::Render ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuRender>(
				nullptr, "contextualMenuRender" );
		_menus[ int( Menu::Viewpoint ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuViewpoint>(
				nullptr, "ContextualMenuViewpoint" );
	}

} // namespace VTX::UI
