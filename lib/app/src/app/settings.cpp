#include "app/settings.hpp"
#include "app/core/settings/settings_system.hpp"

namespace VTX::App::Settings
{
	void initSettings()
	{
		auto & settings = SETTINGS_SYSTEM();

		// Camera.
		settings.referenceSetting( Camera::NEAR_CLIP_KEY, Camera::NEAR_DEFAULT );
		settings.referenceSetting( Camera::FAR_CLIP_KEY, Camera::FAR_DEFAULT );
		settings.referenceSetting( Camera::FOV_KEY, Camera::FOV_DEFAULT );
		settings.referenceSetting( Camera::PROJECTION_KEY, Camera::PROJECTION_DEFAULT );

		// Controllers.
		settings.referenceSetting( Controller::TRANSLATION_SPEED_KEY, Controller::TRANSLATION_SPEED_DEFAULT );
		settings.referenceSetting( Controller::ACCELERATION_FACTOR_KEY, Controller::ACCELERATION_FACTOR_DEFAULT );
		settings.referenceSetting( Controller::DECELERATION_FACTOR_KEY, Controller::DECELERATION_FACTOR_DEFAULT );
		settings.referenceSetting( Controller::ROTATION_SPEED_KEY, Controller::ROTATION_SPEED_DEFAULT );
		settings.referenceSetting( Controller::INVERT_Y_KEY, Controller::INVERT_Y_DEFAULT );
		settings.referenceSetting( Controller::ELASTICITY_ACTIVE_KEY, Controller::ELASTICITY_ACTIVE_DEFAULT );
		settings.referenceSetting( Controller::ELASTICITY_FACTOR_KEY, Controller::ELASTICITY_FACTOR_DEFAULT );

		// Selection.
		settings.referenceSetting( Selection::MOLECULE_GRANULARITY_KEY, Selection::MOLECULE_GRANULARITY_DEFAULT );
	}
} // namespace VTX::App::Settings
