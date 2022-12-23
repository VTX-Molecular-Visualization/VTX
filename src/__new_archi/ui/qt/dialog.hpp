#ifndef __VTX_UI_QT_DIALOG__
#define __VTX_UI_QT_DIALOG__

#include "action/base_action.hpp"
#include "worker/worker_manager.hpp"
#include <QString>
#include <QWidget>

namespace VTX
{
	namespace IO::Struct
	{
		class ImageExport;
	}
	namespace Model
	{
		class Molecule;
	}

	namespace UI::QT
	{
		class MainWindow;
		class Dialog
		{
		  public:
			static void openInformationDialog( const QString & p_title, const QString & p_message );
			static void confirmActionDialog( Action::BaseAction * const p_action,
											 const QString &			p_title,
											 const QString &			p_message );

			static void openDownloadMoleculeDialog();
			static void openDownloadMoleculeDialog( const QString & p_pdbCode );

			static void openLoadMoleculeDialog();
			static void openExportMoleculeDialog();
			static void openLoadTrajectoryDialog( Model::Molecule & p_target );

			static void createNewSessionDialog();

			static void leavingSessionDialog( Worker::CallbackThread & p_callbackSucceed );

			static void openLoadSessionDialog();
			static void openSaveSessionDialog( Worker::CallbackThread * const p_callback = nullptr );

			static void openAdvancedSettingImageExportDialog();
			static bool openExportImageDialog( const IO::Struct::ImageExport & p_exportData );

			static void importRepresentationPresetDialog();
			static void importRenderEffectPresetDialog();

			static void openGLInitializationFail();
			static void unhandledException();

		  private:
			static QString		_toQPath( const Util::FilePath & p_path );
			static MainWindow * _getMainWindow();
		};

	} // namespace UI::QT
} // namespace VTX

#endif
