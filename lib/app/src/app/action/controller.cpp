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
