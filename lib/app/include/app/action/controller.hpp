#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/services.hpp"
#include "app/setting/controller.hpp"

namespace VTX::App::Action::Controller
{

	/**
	 * @brief Set the camera controller to the requested type.
	 */
	template<Setting::E_CONTROLLER C>
	struct SetCameraController
	{
		void execute()
		{
			const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
			REG().patch<Setting::Controller>( e, []( Setting::Controller & p_setting ) { p_setting.current = C; } );
		}
	};

	/**
	 * @brief Set the camera vertical rotation inversion.
	 */
	struct SetInvertY
	{
		void execute( const bool );
	};

	/**
	 * @brief Set the camera controller translation speed.
	 */
	struct SetTranslationSpeed
	{
		void execute( const float );
	};

	/**
	 * @brief Set the camera controller rotation speed.
	 */
	struct SetRotationSpeed
	{
		void execute( const float );
	};

	/**
	 * @brief Set the camera controller acceleration factor.
	 */
	struct SetAccelerationFactor
	{
		void execute( const float );
	};

	/**
	 * @brief Set the camera controller deceleration factor.
	 */
	struct SetDecelerationFactor
	{
		void execute( const float );
	};

	/**
	 * @brief Enable or disable trackball elasticity.
	 */
	struct SetElasticityActive
	{
		void execute( const bool );
	};

	/**
	 * @brief Set the trackball elasticity factor.
	 */
	struct SetElasticityFactor
	{
		void execute( const float );
	};
} // namespace VTX::App::Action::Controller

#endif
