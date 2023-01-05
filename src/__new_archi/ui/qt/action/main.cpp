#include "main.hpp"
#include "__new_archi/ui/qt/state/state_machine.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"
#include "action/action_manager.hpp"
#include "action/base_action.hpp"
#include "define.hpp"
#include "id.hpp"
#include "tool/logger.hpp"

namespace VTX::UI::QT::Action::Main
{
	void Quit::execute() { QT_APP()->closeAllWindows(); }

	void ToggleCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::State::VISUALIZATION )
			->toggleCameraController();
	}

	void ChangeCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::State::VISUALIZATION )
			->setCameraController( _id );
	};

	void ResetCameraController::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<QT::State::Visualization>( VTX::ID::State::VISUALIZATION )
			->resetCameraController();
	};
} // namespace VTX::UI::QT::Action::Main
