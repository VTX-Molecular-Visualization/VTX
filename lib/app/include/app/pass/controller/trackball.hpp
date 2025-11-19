#ifndef __VTX_APP_PASS_CONTROLLER_TRACKBALL__
#define __VTX_APP_PASS_CONTROLLER_TRACKBALL__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/settings/settings.hpp"
#include <util/constants.hpp>

namespace VTX::App::Pass::Controller
{
	/**
	 * @brief System that manages a trackball camera controller from inputs (move around a specified target).
	 */
	class Trackball : public IPass
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Trackball( const ECS::Entity & p_ent );

		/**
		 * @brief Called each frame.
		 */
		void update( const float, const float );

	  private:
		/**
		 * @brief Controlled camera entity.
		 */
		const ECS::Entity _cameraEntity;

		/**
		 * @brief Controller target to rotate around.
		 */
		Vec3f _target = VEC3F_XYZ;

		/**
		 * @brief Current velocity.
		 */
		Vec3f _velocity = VEC3F_ZERO;

		/**
		 * @brief Update needed.
		 */
		bool _needUpdate = true;

		/**
		 * @brief Controller setting pointers (from SETTINGS()).
		 */
		const float * _translationSpeed;
		const float * _accelerationFactor;
		const float * _decelerationFactor;
		const float * _rotationSpeed;
		const bool *  _invertY;
		const bool *  _elasticityActive;
		const float * _elasticityFactor;

		/**
		 * @brief Update velocity from elasticity.
		 */
		void _updateElasticity( const float & );
	};

} // namespace VTX::App::Pass::Controller

#endif
