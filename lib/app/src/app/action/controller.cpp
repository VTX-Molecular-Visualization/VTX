#include "app/action/controller.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/core/controller/controller_system.hpp"
#include "app/mode/visualization.hpp"
#include "app/vtx_app.hpp"
#include <util/collection.hpp>

namespace VTX::App::Action::Controller
{
	void EnableController ::execute()
	{
		using namespace VTX::App::Controller;
		auto & system = CONTROLLER_SYSTEM();
		if ( _controller == Camera::Trackball::NAME )
		{
			system.enableController<Camera::Trackball>();
		}
		else if ( _controller == Camera::Freefly::NAME )
		{
			system.enableController<Camera::Freefly>();
		}
	}

	void DisableController ::execute()
	{
		using namespace VTX::App::Controller;
		auto & system = CONTROLLER_SYSTEM();
		if ( _controller == Camera::Trackball::NAME )
		{
			system.disableController<Camera::Trackball>();
		}
		else if ( _controller == Camera::Freefly::NAME )
		{
			system.disableController<Camera::Freefly>();
		}
	}

	void ToggleCameraController ::execute()
	{
		using namespace VTX::App::Controller;
		auto & system = CONTROLLER_SYSTEM();
		if ( system.isControllerEnabled<Camera::Trackball>() )
		{
			system.enableController<Camera::Freefly>();
			system.disableController<Camera::Trackball>();
		}
		else
		{
			system.enableController<Camera::Trackball>();
			system.disableController<Camera::Freefly>();
		}
	}

} // namespace VTX::App::Action::Controller
