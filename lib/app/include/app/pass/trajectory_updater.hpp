#ifndef __VTX_APP_PASS_TRAJECTORY_UPDATER__
#define __VTX_APP_PASS_TRAJECTORY_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/system/trajectory.hpp"
#include <unordered_map>
#include <util/players.hpp>
namespace VTX::App::Pass
{
	class TrajectoryUpdater final : public IPass
	{
	  public:
		TrajectoryUpdater();
		void update( const float, const float );

	  private:
		bool _tryUpdateFrame( const Entity & entity, System::TrajectoryFullBuffer & p_traj ) noexcept;

		void _onTrajectoryFullBufferCreation( Entity );
		void _onTrajectoryDestruction( Entity );
	};
} // namespace VTX::App::Pass
#endif
