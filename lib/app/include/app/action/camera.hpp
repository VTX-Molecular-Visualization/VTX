#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/event_hub.hpp>

namespace VTX::App::Action::Camera
{
	template<App::Scene::Camera::PROJECTION P>
	struct SetProjectionMode
	{
		void execute()
		{
			auto & reg = REG();

			ECS::Entity		entity = ECS::getFirstEntityWithComponent<Scene::Camera>();
			Scene::Camera & camera = reg.get<Scene::Camera>( entity );

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
