#include "app/settings/settings.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings_manager.hpp"

namespace VTX::App::Settings
{
	void initSettings()
	{
		auto & settings = SETTINGS();

		// Camera.
		settings.add<float>( Camera::NEAR_CLIP_KEY, 1e-1f, 1e-1f, 1e4f );
		settings.add<float>( Camera::FAR_CLIP_KEY, 1e4f, 1e-1f, 1e4f );
		settings.add<float>( Camera::FOV_KEY, 45.f, 10.f, 90.f );
		settings.add<Scene::Camera::PROJECTION>( Camera::PROJECTION_KEY, Scene::Camera::PROJECTION::PERSPECTIVE );

		// Controllers.
		settings.add<float>( Controller::TRANSLATION_SPEED_KEY, 150.f, 50.f, 300.f );
		settings.add<float>( Controller::ACCELERATION_FACTOR_KEY, 2.f, 1.f, 20.f );
		settings.add<float>( Controller::DECELERATION_FACTOR_KEY, 10.f, 1.f, 50.f );
		settings.add<float>( Controller::ROTATION_SPEED_KEY, 0.005f, 0.001f, 0.01f );
		settings.add<bool>( Controller::INVERT_Y_KEY, false );
		settings.add<bool>( Controller::ELASTICITY_ACTIVE_KEY, true );
		settings.add<float>( Controller::ELASTICITY_FACTOR_KEY, 6.f, 1.f, 40.f );

		// Selection.

		// Network.
	}
} // namespace VTX::App::Settings
