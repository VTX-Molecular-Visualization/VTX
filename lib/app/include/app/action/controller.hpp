#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/controller/freefly.hpp"
#include "app/controller/trackball.hpp"
#include "app/events.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include <renderer/camera.hpp>
#include <util/math/aabb.hpp>

namespace VTX::App::Action::Controller
{
	/**
	 * @brief Available camera controllers.
	 */
	enum struct E_CONTROLLER : uint
	{
		TRACKBALL,
		FREEFLY
	};

	/**
	 * @brief Set the camera controller to the requested type.
	 */
	template<E_CONTROLLER C>
	struct SetCameraController
	{
		void execute()
		{
			ECS::Entity entity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();

			const auto	 entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
			const auto & aabb	  = REG().get<Util::Math::AABB>( entScene );

			// Set controller.
			if constexpr ( C == E_CONTROLLER::TRACKBALL )
			{
				PASS().getPass<Pass::CameraUpdater>()->setController<App::Controller::Trackball>( aabb.centroid() );
			}
			else if constexpr ( C == E_CONTROLLER::FREEFLY )
			{
				PASS().getPass<Pass::CameraUpdater>()->setController<App::Controller::Freefly>();
			}
			else
			{
				static_assert( always_false_v<C>, "Unsupported controller type." );
			}

			HUB().trigger<Events::CameraControllerChange>( toUnderlying( C ) );
		}
	};
} // namespace VTX::App::Action::Controller

#endif
