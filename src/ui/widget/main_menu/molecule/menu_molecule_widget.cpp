#include "menu_molecule_widget.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Molecule
{
	MenuMoleculeWidget::MenuMoleculeWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuMoleculeWidget::~MenuMoleculeWidget() {}

	void MenuMoleculeWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_objectDisplay
			= addToolBlock<Visualization::MenuVisualizationObjectDisplayWidget>( "objectDisplayBlockWidget" );
		_selectionAction
			= addToolBlock<Visualization::MenuVisualizationSelectionActionWidget>( "selectionBlockWidget" );
		_representation
			= WidgetFactory::get().instantiateViewWidget<Visualization::MenuVisualizationRepresentationWidget>(
				&Model::Representation::RepresentationLibrary::get(),
				ID::View::UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY,
				this,
				"RepresentationBlockWidget" );
		addToolBlock( _representation );

		_windows = addToolBlock<Visualization::MenuVisualizationWindowsWidget>( "windowsBlockWidget" );
	}
	void MenuMoleculeWidget::_setupSlots() {}
	void MenuMoleculeWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Molecule
