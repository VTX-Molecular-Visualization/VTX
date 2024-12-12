#include "app/action/controller.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/mode/visualization.hpp"
#include "app/vtx_app.hpp"
#include <util/collection.hpp>

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

	void SetCameraProjectionOrthographic::execute()
	{
		auto & camera = SCENE().getCamera();
		camera.setCameraProjection( Component::Render::Camera::PROJECTION::ORTHOGRAPHIC );
	}

	void SetCameraProjectionPerspective::execute()
	{
		auto & camera = SCENE().getCamera();
		camera.setCameraProjection( Component::Render::Camera::PROJECTION::PERSPECTIVE );
	}

	void ToggleCameraProjection::execute()
	{
		using namespace Component::Render;
		auto & camera = SCENE().getCamera();
		if ( camera.getProjection() == Camera::PROJECTION::ORTHOGRAPHIC )
		{
			camera.setCameraProjection( Camera::PROJECTION::PERSPECTIVE );
		}
		else
		{
			camera.setCameraProjection( Camera::PROJECTION::ORTHOGRAPHIC );
		}
	}

} // namespace VTX::App::Action::Controller
