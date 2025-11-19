#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/math/aabb.hpp>

namespace VTX::App::Action::Camera
{
	/**
	 * @brief Set camera position.
	 */
	struct SetPosition
	{
		void execute( const Vec3f & );
	};

	/**
	 * @brief Set camera rotation (euler angles).
	 */
	struct SetRotation
	{
		void execute( const Vec3f & );
	};

	/**
	 * @brief Set camera scale.
	 */
	struct SetScale
	{
		void execute( const float );
	};

	/**
	 * @brief Set camera projection mode.
	 */
	template<App::Scene::Camera::PROJECTION P>
	struct SetProjectionMode
	{
		void execute()
		{
			auto & reg = REG();

			auto [ entity, camera ] = ECS::getFirstEntityWithComponents<App::Scene::Camera>();

			reg.patch<App::Scene::Camera>(
				entity,
				[]( App::Scene::Camera & c )
				{
					auto & settings = SETTINGS();
					settings.setValue<int>( Settings::Camera::PROJECTION_KEY, int( P ) );
				}
			);

			HUB().trigger<App::Events::CameraProjectionChange<static_cast<int>( P )>>();
		}
	};

	/**
	 * @brief Reset camera instantanly to fit the scene.
	 */
	struct Reset
	{
		void execute();
	};

	/**
	 * @brief Launch animation to orient the camera to fit the target.
	 */
	struct Orient
	{
		/**
		 * @brief Orient on current selection, or scene AABB if no selection.
		 */
		void execute();
		/**
		 * @brief Orient on given AABB.
		 */
		void execute( const Util::Math::AABB & p_target );
	};

} // namespace VTX::App::Action::Camera

#endif
