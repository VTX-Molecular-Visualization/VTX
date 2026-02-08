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
		Trackball( const ECS::Entity &, const Vec3f & = VEC3F_ZERO );

		/**
		 * @brief Called each frame.
		 */
		void update( const float, const float );

		/**
		 * @brief Set the controller target.
		 */
		inline void setTarget( const Vec3f & p_target ) { _target = p_target; }

		/**
		 * @brief Stop camera movement.
		 */
		inline void stop() { _velocity = VEC3F_ZERO; }

	  private:
		/**
		 * @brief Controlled camera entity.
		 */
		const ECS::Entity _cameraEntity;

		/**
		 * @brief Controller target to rotate around.
		 */
		Vec3f _target = VEC3F_ZERO;

		/**
		 * @brief Current velocity.
		 */
		Vec3f _velocity = VEC3F_ZERO;

		/**
		 * @brief Update needed.
		 */
		bool _needUpdate = true;

		/**
		 * @brief Controller settings.
		 */
		float translationSpeed;
		float accelerationFactor;
		float decelerationFactor;
		float rotationSpeed;
		bool  invertY;
		bool  elasticityActive;
		float elasticityFactor;

		/**
		 * @brief Update velocity from elasticity.
		 */
		void _updateElasticity( const float & );
	};

} // namespace VTX::App::Pass::Controller

#endif
