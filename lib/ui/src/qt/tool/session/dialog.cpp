#include "ui/qt/tool/session/dialog.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/session/widget/dialog/download_molecule_dialog.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/main.hpp>
#include <app/old_app/io/struct/scene_path_data.hpp>
// #include <app/old_app/selection/selection_manager.hpp>
#include <app/old_app/vtx_app.hpp>
// #include "ui/old_ui/util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <app/old_app/io/filesystem.hpp>
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
		QString defaultFilter = QString::fromStdString( IO::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString defaultPath	  = QString::fromStdString( Setting::getLastImportedMoleculeFolder() );

		const QStringList filenames
			= QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
											 "Open molecule",
											 defaultPath,
											 QString::fromStdString( IO::Filesystem::LOAD_MOLECULE_FILTERS ),
											 &defaultFilter );

		if ( !filenames.isEmpty() )
		{
			Setting::saveLastImportedMoleculeFolder( filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::Open( filepathes ) );
		}
	}
	void Dialog::openExportMoleculeDialog()
	{
		QString		  defaultFilter = QString::fromStdString( IO::Filesystem::DEFAULT_MOLECULE_WRITE_FILTER );
		const QString defaultPath	= QString::fromStdString( IO::Filesystem::getDefaultMoleculeExportPath().string() );

		const QString filename
			= QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
											"Export molecule",
											defaultPath,
											QString::fromStdString( IO::Filesystem::EXPORT_MOLECULE_FILTERS ),
											&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			Setting::saveLastExportedMoleculeFolder( directoryPath.string() );
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
		//	VTX_ACTION( new Action::Main::Open( FilePath( filename.toStdString() ), p_target ) );
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
			const FilePath & filepath = VTXApp::get().getScenePathData().getCurrentPath();

			Worker::CallbackThread * threadCallback = new Worker::CallbackThread( p_callback );

			if ( filepath.empty() )
			{
				Dialog::openSaveSessionDialog( threadCallback );
			}
			else
			{
				VTX_ACTION( new Action::Main::Save( FilePath( filepath ), threadCallback ) );
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
		QString		  defaultFilter = QString::fromStdString( IO::Filesystem::DEFAULT_FILE_WRITE_FILTER );
		const QString defaultPath	= QString::fromStdString( IO::Filesystem::getDefaultSceneSavePath().string() );

		const QString filename
			= QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
											"Save session",
											defaultPath,
											QString::fromStdString( IO::Filesystem::SAVE_SCENE_FILTERS ),
											&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			Setting::saveLastSavedSessionFolder( directoryPath.string() );
			VTX_ACTION( new Action::Main::Save( path, p_callback ) );
		}
	}
	void Dialog::openLoadSessionDialog()
	{
		QString defaultFilter = QString::fromStdString( IO::Filesystem::DEFAULT_FILE_READ_FILTER );
		QString defaultPath	  = QString::fromStdString( Setting::getLastLoadedSessionFolder() );

		const QStringList filenames
			= QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
											 "Open session",
											 defaultPath,
											 QString::fromStdString( IO::Filesystem::OPEN_FILE_FILTERS ),
											 &defaultFilter );

		if ( filenames.size() > 0 )
		{
			Setting::saveLastLoadedSessionFolder( filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::Open( filepathes ) );
		}
	}
} // namespace VTX::UI::QT::Tool::Session::Dialog
