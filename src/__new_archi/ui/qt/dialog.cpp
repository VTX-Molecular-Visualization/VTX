#include "dialog.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "action/action_manager.hpp"
#include "src/action/main.hpp"
// #include "io/struct/image_export.hpp"
// #include "io/struct/scene_path_data.hpp"
// #include "selection/selection_manager.hpp"
// #include "__new_archi/ui/qt/widget/dialog/download_molecule_dialog.hpp"
#include "vtx_app.hpp"
// #include "ui/widget/dialog/image_exporter.hpp"
// #include "util/ui.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QStringList>
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
	// const QString	defaultPath	  = Setting::getLastExportedImageFolder();

	// const QString filepath = QFileDialog::getSaveFileName(
	//	_getMainWindow(), "Export image", defaultPath, Util::Filesystem::IMAGE_EXPORT_EXTENSIONS, defaultFilter );

	// if ( !filepath.isNull() )
	//{
	//	const Util::FilePath path		   = Util::FilePath( filepath.toStdString() );
	//	const Util::FilePath directoryPath = Util::Filesystem::getParentDir( path );

	//	Setting::saveLastExportedImageFolder( QString::fromStdString( directoryPath.path() ) );
	//	VTX_ACTION(
	//		new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL, filepath.toStdString(), p_exportData ) );

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
	//	std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
	//	for ( const QString & qstr : filenames )
	//		filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

	//	VTX_ACTION( new Action::Main::ImportRepresentationPreset( filepathes ) );
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
	//	std::vector<Util::FilePath> filepathes = std::vector<Util::FilePath>();
	//	for ( const QString & qstr : filenames )
	//		filepathes.emplace_back( Util::FilePath( qstr.toStdString() ) );

	//	VTX_ACTION( new Action::Main::ImportRenderEffectPreset( filepathes ) );
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

		VTXApp::get().exit( EXIT_FAILURE );
	}

	void Dialog::unhandledException()
	{
		const std::exception e = std::exception( "Unhandlded error has occured" );
		exceptionDialog( e );
	}

	// QString Dialog::_toQPath( const Util::FilePath & p_path ) { return QString::fromStdString( p_path.path() ); }

} // namespace VTX::UI::QT
