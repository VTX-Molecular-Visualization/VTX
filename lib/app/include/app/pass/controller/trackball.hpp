#ifndef __VTX_APP_PASS_CONTROLLER_TRACKBALL__
#define __VTX_APP_PASS_CONTROLLER_TRACKBALL__

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
		Trackball( const ECS::Entity & p_ent );

		void update( const float, const float );

	  private:
		const ECS::Entity _cameraEntity;
		Vec3f			  _target	  = VEC3F_XYZ;
		Vec3f			  _velocity	  = VEC3F_ZERO;
		bool			  _needUpdate = true;

		const float * _translationSpeed;
		const float * _accelerationFactor;
		const float * _decelerationFactor;
		const float * _rotationSpeed;
		const bool *  _invertY;
		const bool *  _elasticityActive;
		const float * _elasticityFactor;

		void _updateElasticity( const float & );
	};

} // namespace VTX::App::Pass::Controller

#endif
