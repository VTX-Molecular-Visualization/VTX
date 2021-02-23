#include "contextual_menu.hpp"
#include "ui/widget/contextual_menu/contextual_menu_atom.hpp"
#include "ui/widget/contextual_menu/contextual_menu_chain.hpp"
#include "ui/widget/contextual_menu/contextual_menu_instantiated_representation.hpp"
#include "ui/widget/contextual_menu/contextual_menu_molecule.hpp"
#include "ui/widget/contextual_menu/contextual_menu_residue.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI
{
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
		_menus[ int( Menu::InstantiatedRepresentation ) ]
			= WidgetFactory::get().instantiateWidget<Widget::ContextualMenu::ContextualMenuInstantiatedRepresentation>(
				nullptr, "contextualMenuInstantiatedRepresentation" );
	}

} // namespace VTX::UI