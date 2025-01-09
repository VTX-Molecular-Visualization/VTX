#include "app/mode/visualization.hpp"
#include "app/action/controller.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/controller/picker/selection.hpp"
#include "app/core/action/action_system.hpp"

namespace VTX::App::Mode
{

	void Visualization::enter()
	{
		App::ACTION_SYSTEM().execute<Action::Controller::EnableController<Controller::Camera::Trackball>>();
		App::ACTION_SYSTEM().execute<Action::Controller::EnableController<Controller::Picker::Selection>>();
	}

	void Visualization::exit()
	{
		App::ACTION_SYSTEM().execute<Action::Controller::DisableController<Controller::Camera::Trackball>>();
		App::ACTION_SYSTEM().execute<Action::Controller::DisableController<Controller::Camera::Freefly>>();
		App::ACTION_SYSTEM().execute<Action::Controller::DisableController<Controller::Picker::Selection>>();
	}

	void Visualization::update( const float p_deltaTime, const float p_elapsedTime ) {}
} // namespace VTX::App::Mode
