#include "ui/qt/dialog.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
// 
// #include <app/action/main.hpp>
#include <util/logger.hpp>
// #include <app/old_app/io/struct/image_export.hpp>
// #include <app/old_app/io/struct/scene_path_data.hpp>
// #include <app/application/selection/selection_manager.hpp>
// #include "ui/qt/widget/dialog/download_molecule_dialog.hpp"
// #include "ui/old_ui/ui/widget/dialog/image_exporter.hpp"
// #include "ui/old_ui/util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <string>
#include <util/filesystem.hpp>
#include <vector>

namespace VTX::UI::QT
{
	// void Dialog::openAdvancedSettingImageExportDialog()
	//{
	//  UI::Widget::Dialog::ImageExporter::openDialog();
	//}
	// bool Dialog::openExportImageDialog( const IO::Struct::ImageExport & p_exportData )
	//{
	// QString * const defaultFilter = new QString( Util::Filesystem::getImageExportDefaultFilter().c_str() );
	// const QString	defaultPath	  = VTX::App::Application::Setting::getLastExportedImageFolder();

	// const QString filepath = QFileDialog::getSaveFileName(
	//	_getMainWindow(), "Export image", defaultPath, Util::Filesystem::IMAGE_EXPORT_EXTENSIONS, defaultFilter );

	// if ( !filepath.isNull() )
	//{
	//	const FilePath path		   = FilePath( filepath.toStdString() );
	//	const FilePath directoryPath = Util::Filesystem::getParentDir( path );

	//	VTX::App::Application::Setting::saveLastExportedImageFolder( QString::fromStdString( directoryPath.path() ) );
	//	VTX_ACTION(
	//		new App::Action::Main::Snapshot( Worker::Snapshoter::MODE::GL, filepath.toStdString(), p_exportData ) );

	//	return true;
	//}

	// return false;
	// return false;
	//}

	// void Dialog::importRepresentationPresetDialog()
	//{
	//  const QStringList filenames
	//	= QFileDialog::getOpenFileNames( _getMainWindow(),
	//									 "Import representation preset",
	//									 QString::fromStdString( Util::Filesystem::getExecutableDir().path() ),
	//									 Util::Filesystem::REPRESENTATION_PRESET_FILE_FILTERS );

	// if ( !filenames.isEmpty() )
	//{
	//	std::vector<FilePath> filepathes = std::vector<FilePath>();
	//	for ( const QString & qstr : filenames )
	//		filepathes.emplace_back( FilePath( qstr.toStdString() ) );

	//	VTX_ACTION( new App::Action::Main::ImportRepresentationPreset( filepathes ) );
	//}
	//}
	// void Dialog::importRenderEffectPresetDialog()
	//{
	// const QStringList filenames
	//	= QFileDialog::getOpenFileNames( _getMainWindow(),
	//									 "Import render effect preset",
	//									 QString::fromStdString( Util::Filesystem::getExecutableDir().path() ),
	//									 Util::Filesystem::RENDER_EFFECT_PRESET_FILE_FILTERS );

	// if ( !filenames.isEmpty() )
	//{
	//	std::vector<FilePath> filepathes = std::vector<FilePath>();
	//	for ( const QString & qstr : filenames )
	//		filepathes.emplace_back( FilePath( qstr.toStdString() ) );

	//	VTX_ACTION( new App::Action::Main::ImportRenderEffectPreset( filepathes ) );
	//}
	//}

	void Dialog::exceptionDialog( const std::exception & e )
	{
		std::string msg = "Exception : " + std::string( e.what() );
		msg += '\n';

		msg += "Please open an issue at \n" + VTX_BUG_REPORT_URL + " \nwith your latest file in the /logs directory.";

		VTX_ERROR( msg );

		QMessageBox::critical( &QT_APP()->getMainWindow(),
							   "Error",
							   msg.c_str(),
							   QMessageBox::StandardButton::Ok,
							   QMessageBox::StandardButton::Ok );

		QT_APP()->exit( EXIT_FAILURE );
	}

	void Dialog::unhandledException()
	{
		const std::exception e = std::exception( "Unhandlded error has occured" );
		exceptionDialog( e );
	}

	// QString Dialog::_toQPath( const FilePath & p_path ) { return QString::fromStdString( p_path.path() ); }

} // namespace VTX::UI::QT
