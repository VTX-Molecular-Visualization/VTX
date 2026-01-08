#include "app/settings/settings.hpp"
#include "app/services.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/camera.hpp>

namespace
{
	// Default values.
	constexpr float TRANSLATION_SPEED_DEFAULT = 150.f;
	constexpr float TRANSLATION_SPEED_MIN	  = 50.f;
	constexpr float TRANSLATION_SPEED_MAX	  = 300.f;

	constexpr float ACCELERATION_FACTOR_DEFAULT = 2.f;
	constexpr float ACCELERATION_FACTOR_MIN		= 1.f;
	constexpr float ACCELERATION_FACTOR_MAX		= 20.f;

	constexpr float DECELERATION_FACTOR_DEFAULT = 10.f;
	constexpr float DECELERATION_FACTOR_MIN		= 1.f;
	constexpr float DECELERATION_FACTOR_MAX		= 50.f;

	constexpr float ROTATION_SPEED_DEFAULT = 0.005f;
	constexpr float ROTATION_SPEED_MIN	   = 0.001f;
	constexpr float ROTATION_SPEED_MAX	   = 0.01f;

	constexpr bool INVERT_Y_DEFAULT = false;

	constexpr bool ELASTICITY_ACTIVE_DEFAULT = true;

	constexpr float ELASTICITY_FACTOR_DEFAULT = 6.f;
	constexpr float ELASTICITY_FACTOR_MIN	  = 1.f;
	constexpr float ELASTICITY_FACTOR_MAX	  = 40.f;
} // namespace

namespace VTX::App::Settings
{
	void initSettings()
	{
		auto & settings = SETTINGS();

		// Controllers.
		settings.add<float>(
			Controller::TRANSLATION_SPEED_KEY, TRANSLATION_SPEED_DEFAULT, TRANSLATION_SPEED_MIN, TRANSLATION_SPEED_MAX
		);
		settings.add<float>(
			Controller::ACCELERATION_FACTOR_KEY,
			ACCELERATION_FACTOR_DEFAULT,
			ACCELERATION_FACTOR_MIN,
			ACCELERATION_FACTOR_MAX
		);
		settings.add<float>(
			Controller::DECELERATION_FACTOR_KEY,
			DECELERATION_FACTOR_DEFAULT,
			DECELERATION_FACTOR_MIN,
			DECELERATION_FACTOR_MAX
		);
		settings.add<float>(
			Controller::ROTATION_SPEED_KEY, ROTATION_SPEED_DEFAULT, ROTATION_SPEED_MIN, ROTATION_SPEED_MAX
		);
		settings.add<bool>( Controller::INVERT_Y_KEY, INVERT_Y_DEFAULT );
		settings.add<bool>( Controller::ELASTICITY_ACTIVE_KEY, ELASTICITY_ACTIVE_DEFAULT );
		settings.add<float>(
			Controller::ELASTICITY_FACTOR_KEY, ELASTICITY_FACTOR_DEFAULT, ELASTICITY_FACTOR_MIN, ELASTICITY_FACTOR_MAX
		);

		// Camera.
		using namespace Renderer;
		settings.add<float>( Camera::NEAR_CLIP_KEY, NEAR_CLIP_DEFAULT, NEAR_CLIP_MIN, NEAR_CLIP_MAX );
		settings.add<float>( Camera::FAR_CLIP_KEY, FAR_CLIP_DEFAULT, FAR_CLIP_MIN, FAR_CLIP_MAX );
		settings.add<float>( Camera::FOV_KEY, FOV_DEFAULT, FOV_MIN, FOV_MAX );
		settings.add<PROJECTION>( Camera::PROJECTION_KEY, PROJECTION::PERSPECTIVE );

		// Selection.

		// Network.
	}
} // namespace VTX::App::Settings
