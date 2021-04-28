#include "dialog.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "ui/widget/dialog/download_molecule_dialog.hpp"
#include "util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>

namespace VTX::UI
{
	void Dialog::openDownloadMoleculeDialog() { UI::Widget::Dialog::DownloadMoleculeDialog::openDialog(); }
	void Dialog::openDownloadMoleculeDialog( const QString & p_pdbCode )
	{
		UI::Widget::Dialog::DownloadMoleculeDialog::openDialog( p_pdbCode );
	}

	void Dialog::openLoadMoleculeDialog()
	{
		const QStringList filenames = QFileDialog::getOpenFileNames( &VTXApp::get().getMainWindow(),
																	 "Open molecule",
																	 VTX_SETTING().DEFAULT_MOLECULE_FOLDER,
																	 VTX_SETTING().MOLECULE_FILE_FILTERS );

		if ( !filenames.isEmpty() )
		{
			std::vector<FilePath *> filepathes = std::vector<FilePath *>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( new FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::Open( filepathes ) );
		}
	}
	void Dialog::openExportMoleculeDialog()
	{
		const QString filename = QFileDialog::getSaveFileName( &VTXApp::get().getMainWindow(),
															   "Export molecule",
															   VTX_SETTING().DEFAULT_MOLECULE_FOLDER,
															   VTX_SETTING().MOLECULE_FILE_FILTERS );

		if ( !filename.isNull() )
		{
			FilePath * path = new FilePath( filename.toStdString() );
			VTX_ACTION( new Action::Main::Save( path ) );
		}
	}

	void Dialog::openSaveSessionDialog()
	{
		const QString filename = QFileDialog::getSaveFileName( &VTXApp::get().getMainWindow(),
															   "Save session",
															   VTX_SETTING().DEFAULT_SAVE_FOLDER,
															   VTX_SETTING().SAVE_FILE_FILTERS );

		if ( !filename.isNull() )
		{
			FilePath * path = new FilePath( filename.toStdString() );
			VTX_ACTION( new Action::Main::Save( path ) );
		}
	}
	void Dialog::openLoadSessionDialog()
	{
		const QString filename = QFileDialog::getOpenFileName( &VTXApp::get().getMainWindow(),
															   "Open session",
															   VTX_SETTING().DEFAULT_SAVE_FOLDER,
															   VTX_SETTING().OPEN_FILE_FILTERS );

		if ( !filename.isNull() )
		{
			FilePath * path = new FilePath( filename.toStdString() );
			VTX_ACTION( new Action::Main::Open( path ) );
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
