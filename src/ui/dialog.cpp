#include "dialog.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "ui/widget/dialog/download_molecule_dialog.hpp"
#include <QFileDialog>
#include <QMessageBox>

namespace VTX::UI
{
	void Dialog::openDownloadMoleculeDialog() { UI::Widget::Dialog::DownloadMoleculeDialog::openDialog(); }
	void Dialog::openDownloadMoleculeDialog( const QString & p_pdbCode )
	{
		UI::Widget::Dialog::DownloadMoleculeDialog::openDialog( p_pdbCode );
	}

	void Dialog::openLoadMoleculeDialog( QWidget * const p_caller )
	{
		const QString filename
			= QFileDialog::getOpenFileName( p_caller, "Open Molecule", "", VTX_SETTING().MOLECULE_FILE_FILTERS );

		if ( !filename.isNull() )
		{
			FilePath * const path = new FilePath( filename.toStdString() );
			VTX_ACTION( new Action::Main::Open( path ), true );
		}
	}

	void Dialog::confirmActionDialog( QWidget * const			 p_caller,
									  Action::BaseAction * const p_action,
									  const QString &			 p_title,
									  const QString &			 p_message )
	{
		const int res = QMessageBox::warning( p_caller,
											  p_title,
											  p_message,
											  ( QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No ),
											  QMessageBox::StandardButton::No );

		if ( res == QMessageBox::StandardButton::Yes )
		{
			VTX_ACTION( p_action );
		}
		else
		{
			delete p_action;
		}
	}

} // namespace VTX::UI
