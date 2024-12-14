#ifndef __VTX_APP_CONTROLLER_CAMERA_FREEFLY__
#define __VTX_APP_CONTROLLER_CAMERA_FREEFLY__

#include "app/core/controller/base_controller.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Controller::Camera
{

	class Freefly : public Core::Controller::BaseController
	{
	  public:
		Freefly();

		void update( const float, const float );

		float translationSpeed	 = Settings::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Settings::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Settings::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Settings::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Settings::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Settings::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Settings::Controller::ELASTICITY_FACTOR_DEFAULT;

	  private:
		enum class Keys : int
		{
			MOVE_LEFT,
			MOVE_RIGHT,
			MOVE_FRONT,
			MOVE_BACK,
			MOVE_UP,
			MOVE_DOWN,
		};
	};
} // namespace VTX::App::Controller::Camera
#endif
