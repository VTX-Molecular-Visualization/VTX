#ifndef __VTX_UI_QT_TOOL_SESSION_DIALOG__
#define __VTX_UI_QT_TOOL_SESSION_DIALOG__

#include <QString>
#include <QWidget>
#include <app/old/core/worker/callback.hpp>
#include <app/old/component/chemistry/molecule.hpp>

namespace VTX::UI::QT::Tool::Session::Dialog
{
	void openDownloadMoleculeDialog();
	void openDownloadMoleculeDialog( const QString & p_pdbCode );

	void openLoadMoleculeDialog();
	void openExportMoleculeDialog();
	void openLoadTrajectoryDialog( App::Old::Component::Chemistry::Molecule & p_target );

	void createNewSessionDialog();

	void leavingSessionDialog( VTX::App::Old::Core::Worker::CallbackThread & p_callbackSucceed );

	void openLoadSessionDialog();
	void openSaveSessionDialog( VTX::App::Old::Core::Worker::CallbackThread * const p_callback = nullptr );
} // namespace VTX::UI::QT::Tool::Session::Dialog
#endif
