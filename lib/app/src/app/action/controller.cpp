#include "app/action/controller.hpp"
#include "app/services.hpp"
#include <algorithm>

namespace VTX::App::Action::Controller
{
	void SetInvertY::execute( const bool p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e, [ p_value ]( Setting::Controller & p_setting ) { p_setting.invertY = p_value; }
		);
	}

	void SetTranslationSpeed::execute( const float p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e,
			[ p_value ]( Setting::Controller & p_setting )
			{
				p_setting.translationSpeed
					= std::clamp( p_value, Setting::TRANSLATION_SPEED_MIN, Setting::TRANSLATION_SPEED_MAX );
			}
		);
	}

	void SetRotationSpeed::execute( const float p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e,
			[ p_value ]( Setting::Controller & p_setting )
			{
				p_setting.rotationSpeed
					= std::clamp( p_value, Setting::ROTATION_SPEED_MIN, Setting::ROTATION_SPEED_MAX );
			}
		);
	}

	void SetAccelerationFactor::execute( const float p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e,
			[ p_value ]( Setting::Controller & p_setting )
			{
				p_setting.accelerationFactor
					= std::clamp( p_value, Setting::ACCELERATION_FACTOR_MIN, Setting::ACCELERATION_FACTOR_MAX );
			}
		);
	}

	void SetDecelerationFactor::execute( const float p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e,
			[ p_value ]( Setting::Controller & p_setting )
			{
				p_setting.decelerationFactor
					= std::clamp( p_value, Setting::DECELERATION_FACTOR_MIN, Setting::DECELERATION_FACTOR_MAX );
			}
		);
	}

	void SetElasticityActive::execute( const bool p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e, [ p_value ]( Setting::Controller & p_setting ) { p_setting.elasticityActive = p_value; }
		);
	}

	void SetElasticityFactor::execute( const float p_value )
	{
		const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
		REG().patch<Setting::Controller>(
			e,
			[ p_value ]( Setting::Controller & p_setting )
			{
				p_setting.elasticityFactor
					= std::clamp( p_value, Setting::ELASTICITY_FACTOR_MIN, Setting::ELASTICITY_FACTOR_MAX );
			}
		);
	}
} // namespace VTX::App::Action::Controller
