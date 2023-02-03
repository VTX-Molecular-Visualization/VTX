#include "dialog.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "src/action/action_manager.hpp"
#include "src/action/main.hpp"
// #include "src/io/struct/scene_path_data.hpp"
// #include "selection/selection_manager.hpp"
#include "qt/tool/session/widget/dialog/download_molecule_dialog.hpp"
#include "src/vtx_app.hpp"
// #include "util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <util/filesystem.hpp>
#include <vector>

namespace VTX::UI::QT::Tool::Session::Dialog
{
	void Dialog::openDownloadMoleculeDialog() { Widget::Dialog::DownloadMoleculeDialog::pop(); }
	void Dialog::openDownloadMoleculeDialog( const QString & p_pdbCode )
	{
		Widget::Dialog::DownloadMoleculeDialog::pop( p_pdbCode );
	}

	void Dialog::openLoadMoleculeDialog()
	{
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString			defaultPath	  = Setting::getLastImportedMoleculeFolder();

		const QStringList filenames = QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
																	 "Open molecule",
																	 defaultPath,
																	 Util::Filesystem::LOAD_MOLECULE_FILTERS,
																	 defaultFilter );
		delete defaultFilter;

		if ( !filenames.isEmpty() )
		{
			Setting::saveLastImportedMoleculeFolder( filenames[ filenames.size() - 1 ] );

			std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::Open( filepathes ) );
		}
	}
	void Dialog::openExportMoleculeDialog()
	{
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_WRITE_FILTER );
		const QString	defaultPath = QString::fromStdString( Util::Filesystem::getDefaultMoleculeExportPath().path() );

		const QString filename = QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
															   "Export molecule",
															   defaultPath,
															   Util::Filesystem::EXPORT_MOLECULE_FILTERS,
															   defaultFilter );
		delete defaultFilter;

		if ( !filename.isNull() )
		{
			const Util::FilePath path		   = Util::FilePath( filename.toStdString() );
			const Util::FilePath directoryPath = Util::Filesystem::getParentDir( path );

			Setting::saveLastExportedMoleculeFolder( QString::fromStdString( directoryPath.path() ) );
			VTX_ACTION( new Action::Main::Save( path ) );
		}
	}

	void Dialog::openLoadTrajectoryDialog( Model::Molecule & p_target )
	{
		// QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		// QString			defaultPath	  = Setting::getLastImportedMoleculeFolder();

		// const QString filename = QFileDialog::getOpenFileName( &QT_APP()->getMainWindow(),
		//													   "Open trajectory",
		//													   defaultPath,
		//													   Util::Filesystem::LOAD_TRAJECTORY_FILTERS,
		//													   defaultFilter );
		// delete defaultFilter;

		// if ( !filename.isEmpty() )
		//{
		//	Setting::saveLastImportedMoleculeFolder( filename );
		//	VTX_ACTION( new Action::Main::Open( Util::FilePath( filename.toStdString() ), p_target ) );
		// }
	}

	void Dialog::createNewSessionDialog()
	{
		Worker::CallbackThread callback = Worker::CallbackThread(
			[]( const uint p_code )
			{
				if ( p_code )
					VTX_ACTION( new Action::Main::New() );
			} );

		leavingSessionDialog( callback );
	}
	void Dialog::leavingSessionDialog( Worker::CallbackThread & p_callback )
	{
		if ( !VTXApp::get().hasAnyModifications() )
		{
			p_callback( 1 );
			return;
		}

		const int res = QMessageBox::warning( &QT_APP()->getMainWindow(),
											  "Save session",
											  "All changes on current session will be lost. Continue ?",
											  ( QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard
												| QMessageBox::StandardButton::Cancel ),
											  QMessageBox::StandardButton::Cancel );

		if ( res == QMessageBox::StandardButton::Save )
		{
			const Util::FilePath & filepath = VTXApp::get().getScenePathData().getCurrentPath();

			Worker::CallbackThread * threadCallback = new Worker::CallbackThread( p_callback );

			if ( filepath.empty() )
			{
				Dialog::openSaveSessionDialog( threadCallback );
			}
			else
			{
				VTX_ACTION( new Action::Main::Save( Util::FilePath( filepath ), threadCallback ) );
			}
		}
		else if ( res == QMessageBox::StandardButton::Discard )
		{
			p_callback( 1 );
		}
		else
		{
			p_callback( 0 );
		}
	}

	void Dialog::openSaveSessionDialog( Worker::CallbackThread * const p_callback )
	{
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_FILE_WRITE_FILTER );
		const QString	defaultPath	  = QString::fromStdString( Util::Filesystem::getDefaultSceneSavePath().path() );

		const QString filename = QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
															   "Save session",
															   defaultPath,
															   Util::Filesystem::SAVE_SCENE_FILTERS,
															   defaultFilter );

		delete defaultFilter;

		if ( !filename.isNull() )
		{
			const Util::FilePath path		   = Util::FilePath( filename.toStdString() );
			const Util::FilePath directoryPath = Util::Filesystem::getParentDir( path );

			Setting::saveLastSavedSessionFolder( QString::fromStdString( directoryPath.path() ) );
			VTX_ACTION( new Action::Main::Save( path, p_callback ) );
		}
	}
	void Dialog::openLoadSessionDialog()
	{
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_FILE_READ_FILTER );
		QString			defaultPath	  = Setting::getLastLoadedSessionFolder();

		const QStringList filenames = QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
																	 "Open session",
																	 defaultPath,
																	 Util::Filesystem::OPEN_FILE_FILTERS,
																	 defaultFilter );

		delete defaultFilter;

		if ( filenames.size() > 0 )
		{
			Setting::saveLastLoadedSessionFolder( filenames[ filenames.size() - 1 ] );

			std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::Open( filepathes ) );
		}
	}
} // namespace VTX::UI::QT::Tool::Session::Dialog
