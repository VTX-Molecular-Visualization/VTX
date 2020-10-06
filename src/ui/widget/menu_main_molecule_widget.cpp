#include "menu_main_molecule_widget.hpp"
#include "action/main.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuMainMoleculeWidget::~MenuMainMoleculeWidget() {}

			void MenuMainMoleculeWidget::_loadMoleculeFile()
			{
				// TODO : Filter file type
				const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", VTX_SETTING().MOLECULE_FILE_FILTERS );
				Path *		  path	   = new Path( filename.toStdString() );

				VTX_ACTION( new Action::Main::Open( path ), true );
			}
			void MenuMainMoleculeWidget::_downloadMoleculeFile() { _downloadMoleculeDialog->show(); }

			void MenuMainMoleculeWidget::_saveMoleculeFile() {}

			void MenuMainMoleculeWidget::_setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::_setupUi( p_name );

				_loadMoleculeButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "loadMoleculeButton" );
				_loadMoleculeButton->setData( "Load", ":/sprite/load_molecule_icon.png", Qt::Orientation::Vertical );
				addButton( *_loadMoleculeButton, 0, 0, 1, 1 );

				_downloadMoleculeButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "downloadMoleculeButton" );
				_downloadMoleculeButton->setData( "Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Vertical );
				addButton( *_downloadMoleculeButton, 0, 1, 1, 1 );

				_saveMoleculeButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "saveMoleculeButton" );
				_saveMoleculeButton->setData( "Save", ":/sprite/save_molecule_icon.png", Qt::Orientation::Vertical );
				addButton( *_saveMoleculeButton, 0, 2, 1, 1 );

				validate();

				_downloadMoleculeDialog = WidgetFactory::get().GetWidget<DownloadMoleculeDialog>( this, "downloadMoleculeDialog" );
			}
			void MenuMainMoleculeWidget::_setupSlots()
			{
				_loadMoleculeButton->setTriggerAction( this, &MenuMainMoleculeWidget::_loadMoleculeFile );
				_downloadMoleculeButton->setTriggerAction( this, &MenuMainMoleculeWidget::_downloadMoleculeFile );
				_saveMoleculeButton->setTriggerAction( this, &MenuMainMoleculeWidget::_saveMoleculeFile );
			}
			void MenuMainMoleculeWidget::localize() { setTitle( "Molecule" ); }
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
