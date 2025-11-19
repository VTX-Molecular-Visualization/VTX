#ifndef __VTX_APP_PASS_CONTROLLER_FREEFLY__
#define __VTX_APP_PASS_CONTROLLER_FREEFLY__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/settings/settings.hpp"

namespace VTX::App::Pass::Controller
{
	/**
	 * @brief Freefly camera controller from inputs (free movement in 3D space).
	 */
	class Freefly : public IPass
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Freefly( const ECS::Entity & p_ent );

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
		 * @brief Controller setting pointers (from SETTINGS()).
		 */
		const float * _translationSpeed;
		const float * _accelerationFactor;
		const float * _decelerationFactor;
		const float * _rotationSpeed;
		const bool *  _invertY;
	};
} // namespace VTX::App::Pass::Controller
#endif
