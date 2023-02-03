#include "dialog.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "io/struct/image_export.hpp"
#include "io/struct/scene_path_data.hpp"
#include "selection/selection_manager.hpp"
#include "ui/widget/dialog/download_molecule_dialog.hpp"
#include "ui/widget/dialog/image_exporter.hpp"
#include "util/filesystem.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>
#include <QMessageBox>

namespace VTX::UI
{
	void Dialog::confirmActionDialog( Action::BaseAction * const p_action,
									  const QString &			 p_title,
									  const QString &			 p_message )
	{
		const int res = QMessageBox::warning( &VTXApp::get().getMainWindow(),
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
	void Dialog::openInformationDialog( const QString & p_title, const QString & p_message )
	{
		QMessageBox information = QMessageBox( QMessageBox::Icon::Information,
											   p_title,
											   p_message.toUtf8(),
											   QMessageBox::StandardButton::Ok,
											   &VTXApp::get().getMainWindow() );
		information.setTextFormat( Qt::TextFormat::RichText );

		information.exec();
	}

	void Dialog::openDownloadMoleculeDialog() { UI::Widget::Dialog::DownloadMoleculeDialog::openDialog(); }
	void Dialog::openDownloadMoleculeDialog( const QString & p_pdbCode )
	{
		UI::Widget::Dialog::DownloadMoleculeDialog::openDialog( p_pdbCode );
	}

	void Dialog::openLoadMoleculeDialog()
	{
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString			defaultPath	  = Setting::getLastImportedMoleculeFolder();

		const QStringList filenames = QFileDialog::getOpenFileNames( &VTXApp::get().getMainWindow(),
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

		const QString filename = QFileDialog::getSaveFileName( &VTXApp::get().getMainWindow(),
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
		QString * const defaultFilter = new QString( Util::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString			defaultPath	  = Setting::getLastImportedMoleculeFolder();

		const QString filename = QFileDialog::getOpenFileName( &VTXApp::get().getMainWindow(),
															   "Open trajectory",
															   defaultPath,
															   Util::Filesystem::LOAD_TRAJECTORY_FILTERS,
															   defaultFilter );
		delete defaultFilter;

		if ( !filename.isEmpty() )
		{
			Setting::saveLastImportedMoleculeFolder( filename );
			VTX_ACTION( new Action::Main::Open( Util::FilePath( filename.toStdString() ), p_target ) );
		}
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

		const int res = QMessageBox::warning( &VTXApp::get().getMainWindow(),
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

		const QString filename = QFileDialog::getSaveFileName( &VTXApp::get().getMainWindow(),
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

		const QStringList filenames = QFileDialog::getOpenFileNames( &VTXApp::get().getMainWindow(),
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

	void Dialog::openAdvancedSettingImageExportDialog() { UI::Widget::Dialog::ImageExporter::openDialog(); }
	bool Dialog::openExportImageDialog( const IO::Struct::ImageExport & p_exportData )
	{
		QString * const defaultFilter = new QString( Util::Filesystem::getImageExportDefaultFilter().c_str() );
		const QString	defaultPath	  = Setting::getLastExportedImageFolder();

		const QString filepath = QFileDialog::getSaveFileName( &VTXApp::get().getMainWindow(),
															   "Export image",
															   defaultPath,
															   Util::Filesystem::IMAGE_EXPORT_EXTENSIONS,
															   defaultFilter );

		if ( !filepath.isNull() )
		{
			const Util::FilePath path		   = Util::FilePath( filepath.toStdString() );
			const Util::FilePath directoryPath = Util::Filesystem::getParentDir( path );

			Setting::saveLastExportedImageFolder( QString::fromStdString( directoryPath.path() ) );
			VTX_ACTION(
				new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL, filepath.toStdString(), p_exportData ) );

			return true;
		}

		return false;
	}

	void Dialog::importRepresentationPresetDialog()
	{
		const QStringList filenames
			= QFileDialog::getOpenFileNames( &VTXApp::get().getMainWindow(),
											 "Import representation preset",
											 QString::fromStdString( Util::Filesystem::getExecutableDir().path() ),
											 Util::Filesystem::REPRESENTATION_PRESET_FILE_FILTERS );

		if ( !filenames.isEmpty() )
		{
			std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::ImportRepresentationPreset( filepathes ) );
		}
	}
	void Dialog::importRenderEffectPresetDialog()
	{
		const QStringList filenames
			= QFileDialog::getOpenFileNames( &VTXApp::get().getMainWindow(),
											 "Import render effect preset",
											 QString::fromStdString( Util::Filesystem::getExecutableDir().path() ),
											 Util::Filesystem::RENDER_EFFECT_PRESET_FILE_FILTERS );

		if ( !filenames.isEmpty() )
		{
			std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

			VTX_ACTION( new Action::Main::ImportRenderEffectPreset( filepathes ) );
		}
	}

	void Dialog::openGLInitializationFail()
	{
		std::string msg
			= "Unable to create OpenGL 4.5 context. Update your drivers and check your hardware compatibility.";
		VTX_ERROR( msg );

		QMessageBox::critical( &VTXApp::get().getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );
		std::exit( EXIT_FAILURE );
	}
	void Dialog::unhandledException()
	{
		std::string msg = "An unhandlded error has occured, please open an issue at \n" + VTX_BUG_REPORT_URL
						  + " \nwith your latest file in the /logs directory.";
		VTX_ERROR( msg );

		QMessageBox::critical( &VTXApp::get().getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );
		VTXApp::get().exit( EXIT_FAILURE );
	}

} // namespace VTX::UI
