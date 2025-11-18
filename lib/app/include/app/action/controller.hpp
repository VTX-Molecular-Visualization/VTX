#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/events.hpp"
#include "app/pass/controller/freefly.hpp"
#include "app/pass/controller/trackball.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"

namespace VTX::App::Action::Controller
{
	/**
	 * @brief Set the camera controller to the requested type.
	 */
	template<typename T>
	struct SetCameraController
	{
		void execute()
		{
			ECS::Entity entity = ECS::getFirstEntityOnlyWithComponents<Scene::Camera>();

			// If the requested controller is already active, do nothing.
			if ( PASS().hasPass<T>() )
			{
				return;
			}

			// Remove existing controller passes.
			PASS().removePass<Pass::Controller::Freefly>();
			PASS().removePass<Pass::Controller::Trackball>();

			// Add controller pass.
			PASS().addPass<T>( entity );
			HUB().trigger<Events::CameraControllerChange<T>>();
		}
	};
} // namespace VTX::App::Action::Controller

#endif
