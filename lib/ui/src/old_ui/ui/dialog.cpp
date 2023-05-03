#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/dialog/download_molecule_dialog.hpp"
#include "ui/old_ui/ui/widget/dialog/image_exporter.hpp"
#include "ui/old_ui/ui/widget/dialog/set_trajectory_target_dialog.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <app/action/main.hpp>
#include <app/application/define.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/internal/io/filesystem.hpp>
#include <app/internal/io/serialization/image_export.hpp>
#include <app/internal/io/serialization/scene_path_data.hpp>

namespace VTX::UI
{
	void Dialog::confirmActionDialog( VTX::App::Core::Action::BaseAction * const p_action,
									  const QString &							 p_title,
									  const QString &							 p_message )
	{
		const int res = QMessageBox::warning( &UI::VTXApp::get().getMainWindow(),
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
											   &UI::VTXApp::get().getMainWindow() );
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
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString defaultPath = QString::fromStdString( VTX::App::Application::Setting::getLastImportedMoleculeFolder() );

		const QStringList filenames = QFileDialog::getOpenFileNames(
			&UI::VTXApp::get().getMainWindow(),
			"Open molecule",
			defaultPath,
			QString::fromStdString( App::Internal::IO::Filesystem::LOAD_MOLECULE_FILTERS ),
			&defaultFilter );

		if ( !filenames.isEmpty() )
		{
			VTX::App::Application::Setting::saveLastImportedMoleculeFolder(
				filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Action::Main::Open( filepathes ) );
		}
	}
	void Dialog::openExportMoleculeDialog()
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_MOLECULE_WRITE_FILTER );
		const QString defaultPath
			= QString::fromStdString( App::Internal::IO::Filesystem::getDefaultMoleculeExportPath().string() );

		const QString filename = QFileDialog::getSaveFileName(
			&UI::VTXApp::get().getMainWindow(),
			"Export molecule",
			defaultPath,
			QString::fromStdString( App::Internal::IO::Filesystem::EXPORT_MOLECULE_FILTERS ),
			&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			VTX::App::Application::Setting::saveLastExportedMoleculeFolder( directoryPath.string() );
			VTX_ACTION( new App::Action::Main::Save( path ) );
		}
	}

	void Dialog::openLoadTrajectoryDialog()
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_TRAJECTORY_READ_FILTER );
		QString defaultPath = QString::fromStdString( VTX::App::Application::Setting::getLastImportedMoleculeFolder() );

		const QString filename = QFileDialog::getOpenFileName(
			&UI::VTXApp::get().getMainWindow(),
			"Open trajectory",
			defaultPath,
			QString::fromStdString( App::Internal::IO::Filesystem::LOAD_TRAJECTORY_FILTERS ),
			&defaultFilter );

		if ( !filename.isEmpty() )
		{
			VTX::App::Application::Setting::saveLastImportedMoleculeFolder( filename.toStdString() );
			openSetTrajectoryTargetsDialog( filename.toStdString() );
		}
	}
	void Dialog::openLoadTrajectoryDialog( App::Component::Chemistry::Molecule & p_target )
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_MOLECULE_READ_FILTER );
		QString defaultPath = QString::fromStdString( VTX::App::Application::Setting::getLastImportedMoleculeFolder() );

		const QString filename = QFileDialog::getOpenFileName(
			&UI::VTXApp::get().getMainWindow(),
			"Open trajectory",
			defaultPath,
			QString::fromStdString( App::Internal::IO::Filesystem::LOAD_TRAJECTORY_FILTERS ),
			&defaultFilter );

		if ( !filename.isEmpty() )
		{
			VTX::App::Application::Setting::saveLastImportedMoleculeFolder( filename.toStdString() );
			VTX_ACTION( new App::Action::Main::Open( FilePath( filename.toStdString() ), p_target ) );
		}
	}

	void Dialog::openSetTrajectoryTargetsDialog( const FilePath & p_trajectoryFilePath )
	{
		UI::Widget::Dialog::SetTrajectoryTargetDialog::openDialog( p_trajectoryFilePath );
	}
	void Dialog::openSetTrajectoryTargetsDialog( const std::vector<FilePath> & p_trajectoryFilePaths )
	{
		UI::Widget::Dialog::SetTrajectoryTargetDialog::openDialog( *p_trajectoryFilePaths.begin() );
	}

	void Dialog::createNewSessionDialog()
	{
		VTX::App::Core::Worker::CallbackThread callback = VTX::App::Core::Worker::CallbackThread(
			[]( const uint p_code )
			{
				if ( p_code )
					VTX_ACTION( new App::Action::Main::New() );
			} );

		leavingSessionDialog( callback );
	}
	void Dialog::leavingSessionDialog( VTX::App::Core::Worker::CallbackThread & p_callback )
	{
		if ( !App::VTXApp::get().hasAnyModifications() )
		{
			p_callback( 1 );
			return;
		}

		const int res = QMessageBox::warning( &UI::VTXApp::get().getMainWindow(),
											  "Save session",
											  "All changes on current session will be lost. Continue ?",
											  ( QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard
												| QMessageBox::StandardButton::Cancel ),
											  QMessageBox::StandardButton::Cancel );

		if ( res == QMessageBox::StandardButton::Save )
		{
			const FilePath & filepath = App::VTXApp::get().getScenePathData().getCurrentPath();

			VTX::App::Core::Worker::CallbackThread * threadCallback
				= new VTX::App::Core::Worker::CallbackThread( p_callback );

			if ( filepath.empty() )
			{
				Dialog::openSaveSessionDialog( threadCallback );
			}
			else
			{
				VTX_ACTION( new App::Action::Main::Save( FilePath( filepath ), threadCallback ) );
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

	void Dialog::openSaveSessionDialog( VTX::App::Core::Worker::CallbackThread * const p_callback )
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_FILE_WRITE_FILTER );
		const QString defaultPath
			= QString::fromStdString( App::Internal::IO::Filesystem::getDefaultSceneSavePath().string() );

		const QString filename
			= QFileDialog::getSaveFileName( &UI::VTXApp::get().getMainWindow(),
											"Save session",
											defaultPath,
											QString::fromStdString( App::Internal::IO::Filesystem::SAVE_SCENE_FILTERS ),
											&defaultFilter );

		if ( !filename.isNull() )
		{
			const FilePath path			 = FilePath( filename.toStdString() );
			const FilePath directoryPath = path.parent_path();

			VTX::App::Application::Setting::saveLastSavedSessionFolder( directoryPath.string() );
			VTX_ACTION( new App::Action::Main::Save( path, p_callback ) );
		}
	}
	void Dialog::openLoadSessionDialog()
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::DEFAULT_FILE_READ_FILTER );
		QString defaultPath	  = QString::fromStdString( VTX::App::Application::Setting::getLastLoadedSessionFolder() );

		const QStringList filenames
			= QFileDialog::getOpenFileNames( &UI::VTXApp::get().getMainWindow(),
											 "Open session",
											 defaultPath,
											 QString::fromStdString( App::Internal::IO::Filesystem::OPEN_FILE_FILTERS ),
											 &defaultFilter );

		if ( filenames.size() > 0 )
		{
			VTX::App::Application::Setting::saveLastLoadedSessionFolder(
				filenames[ filenames.size() - 1 ].toStdString() );

			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Action::Main::Open( filepathes ) );
		}
	}

	void Dialog::openAdvancedSettingImageExportDialog() { UI::Widget::Dialog::ImageExporter::openDialog(); }
	bool Dialog::openExportImageDialog( const App::Internal::IO::Serialization::ImageExport & p_exportData )
	{
		QString defaultFilter = QString::fromStdString( App::Internal::IO::Filesystem::getImageExportDefaultFilter() );
		const QString defaultPath
			= QString::fromStdString( VTX::App::Application::Setting::getLastExportedImageFolder() );

		const QString filepath = QFileDialog::getSaveFileName(
			&UI::VTXApp::get().getMainWindow(),
			"Export image",
			defaultPath,
			QString::fromStdString( App::Internal::IO::Filesystem::IMAGE_EXPORT_EXTENSIONS ),
			&defaultFilter );

		if ( !filepath.isNull() )
		{
			FilePath	   path			 = FilePath( filepath.toStdString() );
			const FilePath directoryPath = path.parent_path();

			// If the extension is unknown (or if no extension is set), we append the default extension to the filename
			// to prevent issue in the snapshoter.
			if ( App::Internal::IO::Serialization::ImageExport::getFormatFromExtension( path.extension().string() )
				 == App::Internal::IO::Serialization::ImageExport::Format::UNKNOWN )
			{
				const App::Internal::IO::Serialization::ImageExport::Format defaultFormat
					= VTX_SETTING().getSnapshotFormat();
				const std::string formatStr
					= App::Internal::IO::Serialization::ImageExport::FORMAT_STR[ int( defaultFormat ) ];

				path += '.' + formatStr;
			}

			VTX::App::Application::Setting::saveLastExportedImageFolder( directoryPath.string() );
			VTX_ACTION(
				new App::Action::Main::Snapshot( App::Render::Worker::Snapshoter::MODE::GL, path, p_exportData ) );

			return true;
		}

		return false;
	}

	void Dialog::importRepresentationPresetDialog()
	{
		const QStringList filenames = QFileDialog::getOpenFileNames(
			&UI::VTXApp::get().getMainWindow(),
			"Import representation preset",
			QString::fromStdString( App::Internal::IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() ),
			QString::fromStdString( App::Internal::IO::Filesystem::REPRESENTATION_PRESET_FILE_FILTERS ) );

		if ( !filenames.isEmpty() )
		{
			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Action::Main::ImportRepresentationPreset( filepathes ) );
		}
	}
	void Dialog::importRenderEffectPresetDialog()
	{
		const QStringList filenames = QFileDialog::getOpenFileNames(
			&UI::VTXApp::get().getMainWindow(),
			"Import render effect preset",
			QString::fromStdString( App::Internal::IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() ),
			QString::fromStdString( App::Internal::IO::Filesystem::RENDER_EFFECT_PRESET_FILE_FILTERS ) );

		if ( !filenames.isEmpty() )
		{
			std::vector<FilePath> filepathes = std::vector<FilePath>();
			for ( const QString & qstr : filenames )
				filepathes.emplace_back( FilePath( qstr.toStdString() ) );

			VTX_ACTION( new App::Action::Main::ImportRenderEffectPreset( filepathes ) );
		}
	}

	void Dialog::openGLInitializationFail()
	{
		std::string msg
			= "Unable to create OpenGL 4.5 context. Update your drivers and check your hardware compatibility.";
		VTX_ERROR( msg );

		QMessageBox::critical( &UI::VTXApp::get().getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );
		std::exit( EXIT_FAILURE );
	}
	void Dialog::unhandledException()
	{
		std::string msg = "An unhandlded error has occured, please open an issue at \n"
						  + App::Application::VTX_BUG_REPORT_URL + " \nwith your latest file in the /logs directory.";
		VTX_ERROR( msg );

		QMessageBox::critical( &UI::VTXApp::get().getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );
		UI::VTXApp::get().exit( EXIT_FAILURE );
	}

} // namespace VTX::UI
