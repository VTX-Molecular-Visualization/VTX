#include "app/mode/visualization.hpp"
#include "app/action/controller.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/controller/picker/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/services.hpp"

namespace VTX::App::Mode
{

	void Visualization::enter()
	{
		ACTION().execute<Action::Controller::EnableController<Controller::Camera::Trackball>>();
		ACTION().execute<Action::Controller::EnableController<Controller::Picker::Selection>>();
	}

	void Visualization::exit()
	{
		ACTION().execute<Action::Controller::DisableController<Controller::Camera::Trackball>>();
		ACTION().execute<Action::Controller::DisableController<Controller::Camera::Freefly>>();
		ACTION().execute<Action::Controller::DisableController<Controller::Picker::Selection>>();
	}

	void Visualization::update( const float p_deltaTime, const float p_elapsedTime ) {}
} // namespace VTX::App::Mode
