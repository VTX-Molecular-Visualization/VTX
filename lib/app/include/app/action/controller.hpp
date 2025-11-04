#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/pass/controller/freefly.hpp"
#include "app/pass/controller/trackball.hpp"

namespace VTX::App::Action::Controller
{

	struct ToggleCameraController
	{
		void execute() {}
	};

	template<typename T>
	struct SetCameraController
	{
		void execute() {}
	};
} // namespace VTX::App::Action::Controller

#endif
