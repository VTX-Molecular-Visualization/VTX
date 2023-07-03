#include "ui/qt/action/main.hpp"
#include "ui/id.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/old/core/action/base_action.hpp>
#include <app/old/id.hpp>
#include <app/old/internal/network/request/check_update.hpp>
#include <app/old/network.hpp>

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
			.getState<QT::State::Visualization>( UI::ID::State::VISUALIZATION )
			->toggleCameraController();
	}

	void ChangeCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( UI::ID::State::VISUALIZATION )
			->setCameraController( _id );
	};

	void ResetCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( UI::ID::State::VISUALIZATION )
			->resetCameraController();
	};

	void CheckForUpdate::execute()
	{
		VTX_NETWORK_REQUEST<App::Old::Internal::Network::Request::CheckUpdate>( _showPopupIfNoUpdate );
	}
} // namespace VTX::UI::QT::Action::Main
