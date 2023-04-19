#ifndef __VTX_UI_QT_TOOL_SESSION_DIALOG__
#define __VTX_UI_QT_TOOL_SESSION_DIALOG__

#include <QString>
#include <QWidget>
#include <app/old_app/model/molecule.hpp>
#include <app/core/worker/worker_manager.hpp>

namespace VTX::UI::QT::Tool::Session::Dialog
{
	void openDownloadMoleculeDialog();
	void openDownloadMoleculeDialog( const QString & p_pdbCode );

	void openLoadMoleculeDialog();
	void openExportMoleculeDialog();
	void openLoadTrajectoryDialog( Model::Molecule & p_target );

	void createNewSessionDialog();

	void leavingSessionDialog( VTX::Core::Worker::CallbackThread & p_callbackSucceed );

	void openLoadSessionDialog();
	void openSaveSessionDialog( VTX::Core::Worker::CallbackThread * const p_callback = nullptr );
} // namespace VTX::UI::QT::Tool::Session::Dialog
#endif
