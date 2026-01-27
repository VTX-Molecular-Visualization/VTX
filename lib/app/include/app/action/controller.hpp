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

			// Remove existing controller passes.
			if ( PASS().hasPass<Pass::Controller::Freefly>() )
			{
				if ( C == E_CONTROLLER::FREEFLY )
				{
					return;
				}
				PASS().removePass<Pass::Controller::Freefly>();
			}
			if ( PASS().hasPass<Pass::Controller::Trackball>() )
			{
				if ( C == E_CONTROLLER::TRACKBALL )
				{
					return;
				}
				PASS().removePass<Pass::Controller::Trackball>();
			}

			const auto	 entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
			const auto & aabb	  = REG().get<Util::Math::AABB>( entScene );

			// Add controller pass.
			if constexpr ( C == E_CONTROLLER::TRACKBALL )
			{
				PASS().addPass<Pass::Controller::Trackball>( entity, aabb.centroid() );
			}
			else if constexpr ( C == E_CONTROLLER::FREEFLY )
			{
				PASS().addPass<Pass::Controller::Freefly>( entity );
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
