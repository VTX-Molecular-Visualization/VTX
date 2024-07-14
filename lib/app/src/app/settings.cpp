#include "app/settings.hpp"

namespace VTX::App::Setting
{
	void initSettings( App::Application::Settings::Settings & p_settings )
	{
		// Camera Controller
		p_settings.referenceSetting( Controller::TRANSLATION_SPEED_KEY, Controller::TRANSLATION_SPEED_DEFAULT );
		p_settings.referenceSetting( Controller::ACCELERATION_FACTOR_KEY, Controller::ACCELERATION_FACTOR_DEFAULT );
		p_settings.referenceSetting( Controller::DECELERATION_FACTOR_KEY, Controller::DECELERATION_FACTOR_DEFAULT );
		p_settings.referenceSetting( Controller::ROTATION_SPEED_KEY, Controller::ROTATION_SPEED_DEFAULT );
		p_settings.referenceSetting( Controller::INVERT_Y_KEY, Controller::INVERT_Y_DEFAULT );
		p_settings.referenceSetting( Controller::ELASTICITY_ACTIVE_KEY, Controller::ELASTICITY_ACTIVE_DEFAULT );
		p_settings.referenceSetting( Controller::ELASTICITY_FACTOR_KEY, Controller::ELASTICITY_FACTOR_DEFAULT );
	}
} // namespace VTX::App::Setting