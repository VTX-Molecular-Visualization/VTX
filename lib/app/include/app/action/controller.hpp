#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/events.hpp"
#include "app/pass/controller/freefly.hpp"
#include "app/pass/controller/trackball.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include <renderer/camera.hpp>
#include <util/math/aabb.hpp>

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
			ECS::Entity entity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();

			// If the requested controller is already active, do nothing.
			if ( PASS().hasPass<T>() )
			{
				return;
			}

			// Remove existing controller passes.
			if ( PASS().hasPass<Pass::Controller::Freefly>() )
			{
				PASS().removePass<Pass::Controller::Freefly>();
			}
			if ( PASS().hasPass<Pass::Controller::Trackball>() )
			{
				PASS().removePass<Pass::Controller::Trackball>();
			}

			const auto	 entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
			const auto & aabb	  = REG().get<Util::Math::AABB>( entScene );

			// Add controller pass.
			if constexpr ( std::same_as<T, Pass::Controller::Trackball> )
			{
				PASS().addPass<T>( entity, aabb.centroid() );
			}
			else if constexpr ( std::same_as<T, Pass::Controller::Freefly> )
			{
				PASS().addPass<T>( entity );
			}
			else
			{
				static_assert( always_false_v<T>, "Unsupported controller type." );
			}

			HUB().trigger<Events::CameraControllerChange<T>>();
		}
	};
} // namespace VTX::App::Action::Controller

#endif
