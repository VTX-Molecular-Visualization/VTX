#include "ui/qt/tool/session/dialog.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/session/widget/dialog/download_molecule_dialog.hpp"

#include <app/old/action/main.hpp>
#include <app/old/internal/io/serialization/scene_path_data.hpp>
// #include <app/old/application/selection/selection_manager.hpp>
#include <app/old/vtx_app.hpp>
// #include "ui/old_ui/util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <app/old/internal/io/filesystem.hpp>
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
		QString defaultFilter = QString::fromStdString( App::Old::Internal::IO::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString defaultPath	  = QString::fromStdString( VTX::App::Old::Application::Setting::getLastImportedMoleculeFolder() );

		const QStringList filenames
			= QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
											 "Open molecule",
											 defaultPath,
											 QString::fromStdString( App::Old::Internal::IO::Filesystem::LOAD_MOLECULE_FILTERS ),
											 &defaultFilter );

		if ( !filenames.isEmpty() )
		{
			VTX::App::Old::Application::Setting::saveLastImportedMoleculeFolder( filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Old::Action::Main::Open( filepathes ) );
		}
	}
	void Dialog::openExportMoleculeDialog()
	{
		QString		  defaultFilter = QString::fromStdString( App::Old::Internal::IO::Filesystem::DEFAULT_MOLECULE_WRITE_FILTER );
		const QString defaultPath	= QString::fromStdString( App::Old::Internal::IO::Filesystem::getDefaultMoleculeExportPath().string() );

		const QString filename
			= QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
											"Export molecule",
											defaultPath,
											QString::fromStdString( App::Old::Internal::IO::Filesystem::EXPORT_MOLECULE_FILTERS ),
											&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			VTX::App::Old::Application::Setting::saveLastExportedMoleculeFolder( directoryPath.string() );
			VTX_ACTION( new App::Old::Action::Main::Save( path ) );
		}
	}

	void Dialog::openLoadTrajectoryDialog( App::Old::Component::Chemistry::Molecule & p_target )
	{
		// QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		// QString			defaultPath	  = VTX::App::Old::Application::Setting::getLastImportedMoleculeFolder();

		// const QString filename = QFileDialog::getOpenFileName( &QT_APP()->getMainWindow(),
		//													   "Open trajectory",
		//													   defaultPath,
		//													   Util::Filesystem::LOAD_TRAJECTORY_FILTERS,
		//													   defaultFilter );
		// delete defaultFilter;

		// if ( !filename.isEmpty() )
		//{
		//	VTX::App::Old::Application::Setting::saveLastImportedMoleculeFolder( filename );
		//	VTX_ACTION( new App::Old::Action::Main::Open( FilePath( filename.toStdString() ), p_target ) );
		// }
	}

	void Dialog::createNewSessionDialog()
	{
		VTX::App::Old::Core::Worker::CallbackThread callback = VTX::App::Old::Core::Worker::CallbackThread(
			[]( const uint p_code )
			{
				if ( p_code )
					VTX_ACTION( new App::Old::Action::Main::New() );
			} );

		leavingSessionDialog( callback );
	}
	void Dialog::leavingSessionDialog( VTX::App::Old::Core::Worker::CallbackThread & p_callback )
	{
		if ( !App::Old::VTXApp::get().hasAnyModifications() )
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
			const FilePath & filepath = App::Old::VTXApp::get().getScenePathData().getCurrentPath();

			VTX::App::Old::Core::Worker::CallbackThread * threadCallback = new VTX::App::Old::Core::Worker::CallbackThread( p_callback );

			if ( filepath.empty() )
			{
				Dialog::openSaveSessionDialog( threadCallback );
			}
			else
			{
				VTX_ACTION( new App::Old::Action::Main::Save( FilePath( filepath ), threadCallback ) );
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

	void Dialog::openSaveSessionDialog( VTX::App::Old::Core::Worker::CallbackThread * const p_callback )
	{
		QString		  defaultFilter = QString::fromStdString( App::Old::Internal::IO::Filesystem::DEFAULT_FILE_WRITE_FILTER );
		const QString defaultPath	= QString::fromStdString( App::Old::Internal::IO::Filesystem::getDefaultSceneSavePath().string() );

		const QString filename
			= QFileDialog::getSaveFileName( &QT_APP()->getMainWindow(),
											"Save session",
											defaultPath,
											QString::fromStdString( App::Old::Internal::IO::Filesystem::SAVE_SCENE_FILTERS ),
											&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			VTX::App::Old::Application::Setting::saveLastSavedSessionFolder( directoryPath.string() );
			VTX_ACTION( new App::Old::Action::Main::Save( path, p_callback ) );
		}
	}
	void Dialog::openLoadSessionDialog()
	{
		QString defaultFilter = QString::fromStdString( App::Old::Internal::IO::Filesystem::DEFAULT_FILE_READ_FILTER );
		QString defaultPath	  = QString::fromStdString( VTX::App::Old::Application::Setting::getLastLoadedSessionFolder() );

		const QStringList filenames
			= QFileDialog::getOpenFileNames( &QT_APP()->getMainWindow(),
											 "Open session",
											 defaultPath,
											 QString::fromStdString( App::Old::Internal::IO::Filesystem::OPEN_FILE_FILTERS ),
											 &defaultFilter );

		if ( filenames.size() > 0 )
		{
			VTX::App::Old::Application::Setting::saveLastLoadedSessionFolder( filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Old::Action::Main::Open( filepathes ) );
		}
	}
} // namespace VTX::UI::QT::Tool::Session::Dialog
