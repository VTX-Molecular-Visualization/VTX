#include "menu_home_molecule_widget.hpp"
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
			namespace MainMenu
			{
				namespace Home
				{
					MenuHomeMoleculeWidget::~MenuHomeMoleculeWidget() {}

					void MenuHomeMoleculeWidget::_loadMoleculeFile()
					{
						// TODO : Filter file type
						const QString filename = QFileDialog::getOpenFileName(
							this, "Open Molecule", "", VTX_SETTING().MOLECULE_FILE_FILTERS );

						if ( filename.isEmpty() )
						{
							return;
						}

						FilePath * const path = new FilePath( filename.toStdString() );

						VTX_ACTION( new Action::Main::Open( path ), true );
					}
					void MenuHomeMoleculeWidget::_downloadMoleculeFile() { _downloadMoleculeDialog->show(); }

					void MenuHomeMoleculeWidget::_saveMoleculeFile() {}

					void MenuHomeMoleculeWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						_loadMoleculeButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "loadMoleculeButton" );
						_loadMoleculeButton->setData(
							"Load", ":/sprite/load_molecule_icon.png", Qt::Orientation::Vertical );
						pushButton( *_loadMoleculeButton, 0 );

						_downloadMoleculeButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "downloadMoleculeButton" );
						_downloadMoleculeButton->setData(
							"Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Vertical );
						pushButton( *_downloadMoleculeButton, 1 );

						_saveMoleculeButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "saveMoleculeButton" );
						_saveMoleculeButton->setData(
							"Save", ":/sprite/save_molecule_icon.png", Qt::Orientation::Vertical );
						pushButton( *_saveMoleculeButton, 2 );

						validate();

						_downloadMoleculeDialog
							= WidgetFactory::get().instantiateWidget<Dialog::DownloadMoleculeDialog>(
								this, "downloadMoleculeDialog" );
					}
					void MenuHomeMoleculeWidget::_setupSlots()
					{
						_loadMoleculeButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_loadMoleculeFile );
						_downloadMoleculeButton->setTriggerAction( this,
																   &MenuHomeMoleculeWidget::_downloadMoleculeFile );
						_saveMoleculeButton->setTriggerAction( this, &MenuHomeMoleculeWidget::_saveMoleculeFile );
					}
					void MenuHomeMoleculeWidget::localize() { setTitle( "Molecule" ); }
				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
