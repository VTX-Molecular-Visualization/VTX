#include "ui/old_ui/action/main.hpp"
// #include "ui/old_ui/controller/measurement_picker.hpp"
#include "ui/old_ui/state/state_machine.hpp"
#include "ui/old_ui/state/visualization.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/core/mvc/mvc_manager.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/internal/network/request/check_update.hpp>
#include <app/internal/network/request/download_mmtf.hpp>
#include <app/event.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <app/old_app/io/struct/scene_path_data.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/setting.hpp>
#include <app/old_app/util/molecule.hpp>
#include <app/worker/loader.hpp>
#include <app/worker/render_effect_loader.hpp>
#include <app/worker/representation_loader.hpp>
#include <app/worker/saver.hpp>
#include <app/worker/scene_loader.hpp>
#include <util/logger.hpp>

namespace VTX::Action::Main
{
	void ChangeSelectionGranularity::execute()
	{
		State::Visualization * const state
			= UI::VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != ID::Controller::PICKER )
			state->setPickerController( ID::Controller::PICKER );

		VTX_SETTING().setSelectionGranularity( _granularity );
	}
	void ChangePicker::execute()
	{
		State::Visualization * const state
			= UI::VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != _pickerController )
			state->setPickerController( _pickerController );

		if ( _mode > -1 )
		{
			if ( _pickerController == ID::Controller::MEASUREMENT )
			{
				// Controller::MeasurementPicker * const measurementController
				//	= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

				// measurementController->setCurrentMode( Controller::MeasurementPicker::Mode( _mode ) );
			}
		}
	}

} // namespace VTX::Action::Main
