#ifndef __VTX_APP_PASS_CONTROLLER_TRACKBALL__
#define __VTX_APP_PASS_CONTROLLER_TRACKBALL__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"
#include "app/setting/controller.hpp"
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
		 * @brief Called each frame.
		 */
		bool update( const float, const Setting::Controller &, Util::Math::Transform &, Vec3f & );

		/**
		 * @brief Stop movement.
		 */
		void stop() override { _velocity = VEC3F_ZERO; }

	  private:
		/**
		 * @brief Current velocity.
		 */
		Vec3f _velocity = VEC3F_ZERO;

		/**
		 * @brief Update needed.
		 */
		bool _needUpdate = true;

		/**
		 * @brief Update velocity from elasticity.
		 */
		void _updateVelocity( const float, const float );
	};

} // namespace VTX::App::Controller

#endif
