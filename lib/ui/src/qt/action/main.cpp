#include "ui/qt/action/main.hpp"
#include "ui/id.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/core/action/base_action.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/network/network_manager.hpp>
#include <app/old_app/network/request/check_update.hpp>

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
