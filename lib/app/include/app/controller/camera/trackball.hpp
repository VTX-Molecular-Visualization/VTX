#ifndef __VTX_APP_CONTROLLER_CAMERA_TRACKBALL__
#define __VTX_APP_CONTROLLER_CAMERA_TRACKBALL__

#include "app/core/controller/base_controller_camera.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <app/component/render/camera.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Camera
{

	class Trackball : public Core::Controller::BaseControllerCamera
	{
	  public:
		Trackball();

		void update( const float, const float );

		// void  reset();
		// Vec3f targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const;

		float translationSpeed	 = Settings::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Settings::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Settings::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Settings::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Settings::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Settings::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Settings::Controller::ELASTICITY_FACTOR_DEFAULT;

	  private:
		inline static const float _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;

		enum class Keys : int
		{
			MOVE_FRONT,
			MOVE_BACK,
			ROTATE_LEFT,
			ROTATE_RIGHT,
			ROTATE_UP,
			ROTATE_DOWN,
			ROLL_LEFT,
			ROLL_RIGHT,
		};

		Vec3f _velocity	  = VEC3F_ZERO;
		bool  _needUpdate = false;

		void _updateElasticity( const float & );
	};
} // namespace VTX::App::Controller::Camera
#endif
