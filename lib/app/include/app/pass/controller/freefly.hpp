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
		Freefly( const ECS::Entity & p_ent );

		void update( const float, const float );

	  private:
		const ECS::Entity _cameraEntity;

		const float * _translationSpeed;
		const float * _accelerationFactor;
		const float * _decelerationFactor;
		const float * _rotationSpeed;
		const bool *  _invertY;
	};
} // namespace VTX::App::Pass::Controller
#endif
