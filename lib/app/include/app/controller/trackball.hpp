#ifndef __VTX_APP_PASS_CONTROLLER_TRACKBALL__
#define __VTX_APP_PASS_CONTROLLER_TRACKBALL__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"
#include "app/settings/settings.hpp"
#include <util/constants.hpp>

namespace VTX::App::Controller
{

	/**
	 * @brief Object that manages a trackball camera controller from inputs (move around a specified target).
	 */
	class Trackball : public IController
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Trackball( const Vec3f & = VEC3F_ZERO );

		/**
		 * @brief Called each frame.
		 */
		void update( const float, Util::Math::Transform & );

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
		float _translationSpeed;
		float _accelerationFactor;
		float _decelerationFactor;
		float _rotationSpeed;
		bool  _invertY;
		bool  _elasticityActive;
		float _elasticityFactor;

		/**
		 * @brief Update velocity from elasticity.
		 */
		void _updateElasticity( const float & );
	};

} // namespace VTX::App::Controller

#endif
