#ifndef __VTX_APP_PASS_CONTROLLER_FREEFLY__
#define __VTX_APP_PASS_CONTROLLER_FREEFLY__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/settings/settings.hpp"

namespace VTX::App::Pass::Controller
{

	class Freefly : public IPass
	{
	  public:
		float translationSpeed	 = Settings::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Settings::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Settings::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Settings::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Settings::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Settings::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Settings::Controller::ELASTICITY_FACTOR_DEFAULT;

		Freefly( const ECS::Entity & p_ent ) : _cameraEntity( p_ent ) {}

		void update( const float, const float );

	  private:
		const ECS::Entity _cameraEntity;
	};
} // namespace VTX::App::Pass::Controller
#endif
