#ifndef __VTX_APP_CONTROLLER_FREEFLY__
#define __VTX_APP_CONTROLLER_FREEFLY__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"

namespace VTX::App::Controller
{

	/**
	 * @brief Freefly camera controller from inputs (free movement in 3D space).
	 */
	class Freefly : public IController
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Freefly();

		/**
		 * @brief Called each frame.
		 */
		bool update( const float, Util::Math::Transform &, Vec3f & );

	  private:
		/**
		 * @brief Controller settings.
		 */
		float _translationSpeed;
		float _accelerationFactor;
		float _decelerationFactor;
		float _rotationSpeed;
		bool  _invertY;
	};
} // namespace VTX::App::Controller
#endif
