#ifndef __VTX_APP_SETTING_CONTROLLER__
#define __VTX_APP_SETTING_CONTROLLER__

#include <string>
#include <util/types.hpp>

namespace VTX::App::Setting
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
	 * @brief Default values.
	 */
	constexpr E_CONTROLLER CONTROLLER_DEFAULT = E_CONTROLLER::TRACKBALL;

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

	/**
	 * @brief Controller settings.
	 */
	struct Controller
	{
		E_CONTROLLER current			= CONTROLLER_DEFAULT;
		float		 translationSpeed	= TRANSLATION_SPEED_DEFAULT;
		float		 accelerationFactor = ACCELERATION_FACTOR_DEFAULT;
		float		 decelerationFactor = DECELERATION_FACTOR_DEFAULT;
		float		 rotationSpeed		= ROTATION_SPEED_DEFAULT;
		bool		 invertY			= INVERT_Y_DEFAULT;
		bool		 elasticityActive	= ELASTICITY_ACTIVE_DEFAULT;
		float		 elasticityFactor	= ELASTICITY_FACTOR_DEFAULT;
	};

} // namespace VTX::App::Setting

#endif
