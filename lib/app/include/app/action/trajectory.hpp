#ifndef __VTX_APP_ACTION_TRAJECTORY__
#define __VTX_APP_ACTION_TRAJECTORY__

#include "app/ecs.hpp"

namespace VTX::App::Action::Trajectory
{
	struct ToggleStartPause
	{
	  public:
		void execute( ECS::Entity ) noexcept;
	};
} // namespace VTX::App::Action::Trajectory

#endif
