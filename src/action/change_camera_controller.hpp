#ifndef __VTX_ACTION_CHANGE_CAMERA_CONTROLLER__
#define __VTX_ACTION_CHANGE_CAMERA_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeCameraController : public BaseAction
		{
		  public:
			explicit ChangeCameraController() {}

			virtual void execute() override
			{
				VTXApp::get()
					.getStateMachine()
					.getItem<State::Visualization>( ID::State::VISUALIZATION )
					->toggleController();
			};

		  private:
		};
	} // namespace Action
} // namespace VTX
#endif
