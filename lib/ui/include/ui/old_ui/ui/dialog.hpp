#ifndef __VTX_UI_DIALOG__
#define __VTX_UI_DIALOG__

#include <QString>
#include <QWidget>
#include <app/old/component/chemistry/_fwd.hpp>
#include <app/old/core/action/base_action.hpp>
#include <app/old/core/worker/callback.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX
{
	namespace App::Old::Internal::IO::Serialization
	{
		class ImageExport;
	}

	namespace UI
	{
		class Dialog
		{
		  public:
			static void openInformationDialog( const QString & p_title, const QString & p_message );
			static void confirmActionDialog( VTX::App::Old::Core::Action::BaseAction * const p_action,
											 const QString &							p_title,
											 const QString &							p_message );

			static void openDownloadMoleculeDialog();
			static void openDownloadMoleculeDialog( const QString & p_pdbCode );

			static void openLoadMoleculeDialog();
			static void openExportMoleculeDialog();
			static void openLoadTrajectoryDialog();
			static void openLoadTrajectoryDialog( App::Old::Component::Chemistry::Molecule & p_target );

			static void openSetTrajectoryTargetsDialog( const FilePath & p_trajectoryFilePath );
			static void openSetTrajectoryTargetsDialog( const std::vector<FilePath> & p_trajectoryFilePaths );

			static void createNewSessionDialog();

			static void leavingSessionDialog( VTX::App::Old::Core::Worker::CallbackThread & p_callbackSucceed );

			static void openLoadSessionDialog();
			static void openSaveSessionDialog( VTX::App::Old::Core::Worker::CallbackThread * const p_callback = nullptr );

			static void openAdvancedSettingImageExportDialog();
			static bool openExportImageDialog( const App::Old::Internal::IO::Serialization::ImageExport & p_exportData );

			static void importRepresentationPresetDialog();
			static void importRenderEffectPresetDialog();

			static void openGLInitializationFail();
			static void unhandledException();
		};

	} // namespace UI
} // namespace VTX

#endif
