#ifndef __VTX_APP_PASS_TRACKBALL__
#define __VTX_APP_PASS_TRACKBALL__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/settings/settings.hpp"
#include <util/constants.hpp>

namespace VTX::App::Pass::Controller
{
	/**
	 * @brief System that manages a trackball camera controller from inputs.
	 */
	class Trackball : public IPass
	{
	  public:
		float translationSpeed	 = Settings::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Settings::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Settings::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Settings::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Settings::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Settings::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Settings::Controller::ELASTICITY_FACTOR_DEFAULT;

		Trackball( const ECS::Entity & p_ent ) : _cameraEntity( p_ent )
		{
			// TODO: update on transform change, but only for the relevant entity!
			// REG().on_update<Util::Math::Transform>().connect( []( Util::Math::Transform &) {} );
		}

		void update( const float, const float );

	  private:
		const ECS::Entity _cameraEntity;
		Vec3f			  _target	  = VEC3F_ZERO;
		Vec3f			  _velocity	  = VEC3F_ZERO;
		bool			  _needUpdate = true;

		void _updateElasticity( const float & );
	};

} // namespace VTX::App::Pass::Controller

#endif
