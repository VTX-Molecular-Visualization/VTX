#include "app/action/trajectory.hpp"
#include "app/services.hpp"
#include <functional>

namespace VTX::App::Action::Trajectory
{
	namespace
	{
		void _patch( const Entity p_entity, const std::function<void( System::GenericTrajectory & )> & p_function )
		{
			if ( REG().all_of<System::TrajectoryFullBuffer>( p_entity ) )
			{
				REG().patch<System::TrajectoryFullBuffer>(
					p_entity,
					[ &p_function ]( System::TrajectoryFullBuffer & p_trajectory )
					{ p_function( p_trajectory.genericData ); }
				);
			}
		}
	} // namespace

	void ToggleStartPause::execute( Entity p_entity )
	{
		_patch( p_entity, []( System::GenericTrajectory & traj ) { traj.paused ^= 1; } );
	}

	void SetPaused::execute( Entity p_entity, const bool p_paused )
	{
		_patch( p_entity, [ p_paused ]( System::GenericTrajectory & traj ) { traj.paused = p_paused; } );
	}

	void Stop::execute( Entity p_entity )
	{
		_patch(
			p_entity,
			[]( System::GenericTrajectory & traj )
			{
				traj.player.jumpTo( 0 );
				traj.requestedFrameIndex = 0;
				traj.paused				 = true;
			}
		);
	}

	void JumpTo::execute( Entity p_entity, uint p_step )
	{
		_patch(
			p_entity,
			[ p_step ]( System::GenericTrajectory & traj )
			{
				if ( p_step < traj.trajectorySize )
				{
					traj.player.jumpTo( p_step );
					traj.requestedFrameIndex = p_step;
				}
			}
		);
	}

	void ChangePlayer::execute( Entity p_entity, System::TRAJECTORY_PLAY_MODE p_playerType )
	{
		_patch(
			p_entity,
			[ p_playerType ]( System::GenericTrajectory & traj )
			{
				traj.playMode = p_playerType;
				switch ( p_playerType )
				{
				case System::TRAJECTORY_PLAY_MODE::PING_PONG:
					traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::FORWARD:
					traj.player = Util::Players::Forward( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::FORWARD_LOOP:
					traj.player = Util::Players::ForwardLoop( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::BACKWARD:
					traj.player = Util::Players::Backward( traj.trajectorySize, traj.currentFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::BACKWARD_LOOP:
					traj.player = Util::Players::BackwardLoop( traj.trajectorySize, traj.currentFrameIndex );
					break;
				default: traj.player = Util::Players::PingPong( traj.trajectorySize, traj.currentFrameIndex );
				}
			}
		);
	}

	void ChangeSpeed::execute( Entity p_entity, float p_speed )
	{
		_patch( p_entity, [ p_speed ]( System::GenericTrajectory & traj ) { traj.playingSpeed = p_speed; } );
	}

} // namespace VTX::App::Action::Trajectory
