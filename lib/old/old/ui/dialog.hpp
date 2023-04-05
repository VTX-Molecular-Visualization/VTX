#ifndef __VTX_UI_DIALOG__
#define __VTX_UI_DIALOG__

#include "action/base_action.hpp"
#include "worker/worker_manager.hpp"
#include <QString>
#include <QWidget>
#include <util/filepath.hpp>
#include <vector>

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

	namespace UI
	{
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
			static void openLoadTrajectoryDialog();
			static void openLoadTrajectoryDialog( Model::Molecule & p_target );

			static void openSetTrajectoryTargetsDialog( const FilePath & p_trajectoryFilePath );
			static void openSetTrajectoryTargetsDialog( const std::vector<FilePath> & p_trajectoryFilePaths );

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
		};

	} // namespace UI
} // namespace VTX

#endif
