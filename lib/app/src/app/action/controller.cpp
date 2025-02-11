#include "app/action/controller.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"

namespace VTX::App::Action::Controller
{

	void ToggleCameraController ::execute()
	{
		Component::Controller & component
			= App::ECS_REGISTRY().getComponent<App::Component::Controller>( App::SCENE().getCamera() );

		if ( component.isControllerEnabled<App::Controller::Camera::Trackball>() )
		{
			component.enableController<App::Controller::Camera::Freefly>();
			component.disableController<App::Controller::Camera::Trackball>();
		}
		else
		{
			component.enableController<App::Controller::Camera::Trackball>();
			component.disableController<App::Controller::Camera::Freefly>();
		}
	}

} // namespace VTX::App::Action::Controller
