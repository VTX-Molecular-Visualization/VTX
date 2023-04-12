#include "main.hpp"
#include "action/action_manager.hpp"
#include "controller/measurement_picker.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/filesystem.hpp"
#include "io/struct/scene_path_data.hpp"
#include "mvc/mvc_manager.hpp"
#include "network/network_manager.hpp"
#include "network/request/check_update.hpp"
#include "network/request/download_mmtf.hpp"
#include "object3d/scene.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "util/molecule.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/representation_loader.hpp"
#include "worker/saver.hpp"
#include "worker/scene_loader.hpp"
#include <util/logger.hpp>

namespace VTX::Action::Main
{
	void ChangeSelectionGranularity::execute()
	{
		State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != ID::Controller::PICKER )
			state->setPickerController( ID::Controller::PICKER );

		VTX_SETTING().setSelectionGranularity( _granularity );
	}
	void ChangePicker::execute()
	{
		State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != _pickerController )
			state->setPickerController( _pickerController );

		if ( _mode > -1 )
		{
			if ( _pickerController == ID::Controller::MEASUREMENT )
			{
				Controller::MeasurementPicker * const measurementController
					= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

				measurementController->setCurrentMode( Controller::MeasurementPicker::Mode( _mode ) );
			}
		}
	}

} // namespace VTX::Action::Main
