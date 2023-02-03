#ifndef __VTX_UI_QT_TOOL_SESSION_DIALOG__
#define __VTX_UI_QT_TOOL_SESSION_DIALOG__

#include "src/action/base_action.hpp"
#include "src/worker/worker_manager.hpp"
#include <QString>
#include <QWidget>

namespace VTX::UI::QT::Tool::Session::Dialog
{
	void openDownloadMoleculeDialog();
	void openDownloadMoleculeDialog( const QString & p_pdbCode );

	void openLoadMoleculeDialog();
	void openExportMoleculeDialog();
	void openLoadTrajectoryDialog( Model::Molecule & p_target );

	void createNewSessionDialog();

	void leavingSessionDialog( Worker::CallbackThread & p_callbackSucceed );

	void openLoadSessionDialog();
	void openSaveSessionDialog( Worker::CallbackThread * const p_callback = nullptr );
} // namespace VTX::UI::QT::Tool::Session::Dialog
#endif
