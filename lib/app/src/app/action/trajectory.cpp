#include "app/action/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"

namespace VTX::App::Action::Trajectory
{
	void ToggleStartPause::execute( ECS::Entity p_entity ) noexcept
	{
		System::patchGenericTrajectories( p_entity, []( System::GenericTrajectory & traj ) { traj.paused ^= 1; } );
	}
	void Stop::execute( ECS::Entity p_entity ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity,
			[]( System::GenericTrajectory & traj )
			{
				traj.player.jumpTo( 0 );
				traj.currentFrameIndex = 0;
				traj.paused			   = true;
			}
		);
	}

	void JumpTo::execute( ECS::Entity p_entity, uint p_step ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity,
			[ &p_step ]( System::GenericTrajectory & traj )
			{
				traj.player.jumpTo( p_step );
				traj.currentFrameIndex = p_step;
			}
		);
	}

	void ChangePlayer::execute( ECS::Entity p_entity, System::TrajectoryPlayMode p_playerType ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity,
			[ &p_playerType ]( System::GenericTrajectory & traj )
			{
				switch ( p_playerType )
				{
				case System::TrajectoryPlayMode::pingpong:
					traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TrajectoryPlayMode::forward:
					traj.player = Util::Players::Forward( traj.trajectorySize, traj.currentFrameIndex );
					break;
				default: traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
				}
			}
		);
	}

} // namespace VTX::App::Action::Trajectory
