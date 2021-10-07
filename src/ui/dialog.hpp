#ifndef __VTX_UI_DIALOG__
#define __VTX_UI_DIALOG__

#include "action/base_action.hpp"
#include "worker/worker_manager.hpp"
#include <QString>
#include <QWidget>

namespace VTX::UI
{
	class Dialog
	{
	  public:
		static void openInformationDialog( const QString & p_title, const QString & p_message );

		static void openDownloadMoleculeDialog();
		static void openDownloadMoleculeDialog( const QString & p_pdbCode );

		static void openLoadMoleculeDialog();
		static void openExportMoleculeDialog();

		static void createNewSessionDialog();

		static void leavingSessionDialog( Worker::CallbackThread & p_callbackSucceed );

		static void openLoadSessionDialog();
		static void openSaveSessionDialog( Worker::CallbackThread * const p_callback = nullptr );

		static void importRepresentationPresetDialog();
		static void importRenderEffectPresetDialog();

		static void openGLInitializationFail();
		static void unhandledException();

		static void confirmActionDialog( Action::BaseAction * const p_action,
										 const QString &			p_title,
										 const QString &			p_message );
	};

} // namespace VTX::UI

#endif
