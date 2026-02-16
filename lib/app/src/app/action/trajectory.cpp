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
				traj.requestedFrameIndex = 0;
				traj.paused				 = true;
			}
		);
	}

	void JumpTo::execute( ECS::Entity p_entity, uint p_step ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity,
			[ &p_step ]( System::GenericTrajectory & traj )
			{
				if ( p_step < traj.trajectorySize )
				{
					traj.player.jumpTo( p_step );
					traj.requestedFrameIndex = p_step;
				}
			}
		);
	}

	void ChangePlayer::execute( ECS::Entity p_entity, System::TrajectoryPlayMode p_playerType ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity,
			[ &p_playerType ]( System::GenericTrajectory & traj )
			{
				traj.playMode = p_playerType;
				switch ( p_playerType )
				{
				case System::TrajectoryPlayMode::pingpong:
					traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TrajectoryPlayMode::forward:
					traj.player = Util::Players::Forward( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TrajectoryPlayMode::forwardLoop:
					traj.player = Util::Players::ForwardLoop( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TrajectoryPlayMode::backward:
					traj.player = Util::Players::Backward( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TrajectoryPlayMode::backwardLoop:
					traj.player = Util::Players::BackwardLoop( traj.trajectorySize, traj.currentFrameIndex );
					break;
				default: traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
				}
			}
		);
	}

	void ChangeSpeed::execute( ECS::Entity p_entity, float p_speed ) noexcept
	{
		System::patchGenericTrajectories(
			p_entity, [ &p_speed ]( System::GenericTrajectory & traj ) { traj.playingSpeed = p_speed; }
		);
	}

} // namespace VTX::App::Action::Trajectory
