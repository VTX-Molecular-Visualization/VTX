#ifndef __VTX_UI_INTERNAL_CONTROLLER_CAMERA_FREEFLY__
#define __VTX_UI_INTERNAL_CONTROLLER_CAMERA_FREEFLY__

#include "app/core/controller/base_controller_camera.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Controller::Camera
{

	class Freefly : public Core::Controller::BaseControllerCamera
	{
	  public:
		static inline const Name NAME = "FREEFLY";

		Freefly();

		Name getName() const override { return NAME; }

		void setActive( const bool ) {};
		void update( const float, const float );

		float translationSpeed	 = Setting::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Setting::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Setting::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Setting::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Setting::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Setting::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Setting::Controller::ELASTICITY_FACTOR_DEFAULT;

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
