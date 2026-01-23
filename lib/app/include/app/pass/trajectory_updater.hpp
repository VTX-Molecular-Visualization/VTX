#ifndef __VTX_APP_PASS_TRAJECTORY_UPDATER__
#define __VTX_APP_PASS_TRAJECTORY_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"

namespace VTX::App::Pass
{
	class TrajectoryUpdater final : public IPass
	{
	  public:
		TrajectoryUpdater();
		void update( const float, const float );
	};
} // namespace VTX::App::Pass
#endif
