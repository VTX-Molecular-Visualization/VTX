#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/setting/controller.hpp"

namespace VTX::App::Action::Controller
{

	/**
	 * @brief Set the camera controller to the requested type.
	 */
	template<Setting::E_CONTROLLER C>
	struct SetCameraController
	{
		void execute()
		{
			const auto e = ECS::getFirstEntityOnlyWithComponents<Setting::Controller>();
			REG().patch<Setting::Controller>( e, [ & ]( Setting::Controller & p_setting ) { p_setting.current = C; } );
		}
	};
} // namespace VTX::App::Action::Controller

#endif
