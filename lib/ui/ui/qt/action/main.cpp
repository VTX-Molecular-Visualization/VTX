#include "main.hpp"
#include "id.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include <old/action/action_manager.hpp>
#include <old/action/base_action.hpp>
#include <old/network/network_manager.hpp>
#include <old/network/request/check_update.hpp>

namespace VTX::UI::QT::Action::Main
{
	void Quit::execute() { QT_APP()->closeAllWindows(); }

	void RestoreLayout::execute()
	{
		QT_APP()->getMainWindow().deleteLayoutSaveFile();
		QT_APP()->getMainWindow().restoreDefaultLayout();

		VTX_INFO( "Default layout restored " );
	}

	void ToggleCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->toggleCameraController();
	}

	void ChangeCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->setCameraController( _id );
	};

	void ResetCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::UI_NEW::State::VISUALIZATION )
			->resetCameraController();
	};

	void CheckForUpdate::execute()
	{
		VTX_NETWORK_MANAGER().sendRequest( new Network::Request::CheckUpdate( _showPopupIfNoUpdate ) );
	}
} // namespace VTX::UI::QT::Action::Main
