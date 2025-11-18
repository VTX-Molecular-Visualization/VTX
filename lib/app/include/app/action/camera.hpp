#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/event_hub.hpp>

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

			auto [ entity, camera ] = ECS::getFirstEntityWithComponents<Scene::Camera>();

			reg.patch<Scene::Camera>(
				entity,
				[]( Scene::Camera & c )
				{
					auto & settings = SETTINGS();
					settings.setValue<int>( Settings::Camera::PROJECTION_KEY, int( P ) );
				}
			);

			HUB().trigger<App::Events::CameraProjectionChange<static_cast<int>( P )>>();
		}
	};

} // namespace VTX::App::Action::Camera

#endif
