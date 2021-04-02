#include "menu_home_molecule_widget.hpp"
#include "action/main.hpp"
#include "ui/dialog.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Home
{
	MenuHomeMoleculeWidget::~MenuHomeMoleculeWidget() {}

	void MenuHomeMoleculeWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_loadMoleculeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "loadMoleculeButton" );
		_loadMoleculeButton->setData( "Load", ":/sprite/load_molecule_icon.png", Qt::Orientation::Vertical );
		pushButton( *_loadMoleculeButton, 0 );

		_downloadMoleculeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "downloadMoleculeButton" );
		_downloadMoleculeButton->setData(
			"Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Vertical );
		pushButton( *_downloadMoleculeButton, 1 );

		_saveMoleculeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveMoleculeButton" );
		_saveMoleculeButton->setData( "Save", ":/sprite/save_molecule_icon.png", Qt::Orientation::Vertical );
		pushButton( *_saveMoleculeButton, 2 );

		validate();
	}
	void MenuHomeMoleculeWidget::_setupSlots()
	{
		_loadMoleculeButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_loadMoleculeFile );
		_downloadMoleculeButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_downloadMoleculeFile );
		_saveMoleculeButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_saveMoleculeFile );
	}
	void MenuHomeMoleculeWidget::localize() { setTitle( "Molecule" ); }

	void MenuHomeMoleculeWidget::_loadMoleculeFile() { UI::Dialog::openLoadMoleculeDialog( this ); }
	void MenuHomeMoleculeWidget::_downloadMoleculeFile() { UI::Dialog::openDownloadMoleculeDialog(); }

	void MenuHomeMoleculeWidget::_saveMoleculeFile() {}

} // namespace VTX::UI::Widget::MainMenu::Home
