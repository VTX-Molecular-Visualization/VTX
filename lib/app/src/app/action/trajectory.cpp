#include "app/action/trajectory.hpp"
#include "app/services.hpp"
#include <algorithm>
#include <cmath>
#include <core/struct/trajectory.hpp>
#include <utility>

namespace VTX::App::Action::Trajectory
{
	namespace
	{
		template<typename Func>
		void _patch( const Entity p_entity, Func && p_function )
		{
			if ( REG().all_of<System::TrajectoryPlayer>( p_entity ) )
			{
				REG().patch<System::TrajectoryPlayer>( p_entity, std::forward<Func>( p_function ) );
			}
		}
	} // namespace

	void ToggleStartPause::execute( const Entity p_entity )
	{
		_patch( p_entity, []( System::TrajectoryPlayer & traj ) { traj.paused ^= 1; } );
	}

	void SetPaused::execute( const Entity p_entity, const bool p_paused )
	{
		_patch( p_entity, [ p_paused ]( System::TrajectoryPlayer & traj ) { traj.paused = p_paused; } );
	}

	void Stop::execute( const Entity p_entity )
	{
		_patch(
			p_entity,
			[]( System::TrajectoryPlayer & traj )
			{
				traj.player.jumpTo( 0 );
				traj.requestedFrameIndex = 0;
				traj.paused				 = true;
			}
		);
	}

	void JumpTo::execute( const Entity p_entity, const uint p_step )
	{
		const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
		if ( trajectory == nullptr )
		{
			return;
		}

		_patch(
			p_entity,
			[ p_step, frameCount = trajectory->frameCount ]( System::TrajectoryPlayer & traj )
			{
				if ( p_step < frameCount )
				{
					traj.player.jumpTo( p_step );
					traj.requestedFrameIndex = p_step;
				}
			}
		);
	}

	void ChangePlayer::execute( const Entity p_entity, const System::TRAJECTORY_PLAY_MODE p_playerType )
	{
		const auto * const trajectory = REG().try_get<Core::Struct::Trajectory>( p_entity );
		if ( trajectory == nullptr )
		{
			return;
		}
		const uint frameCount = static_cast<uint>( trajectory->frameCount );

		_patch(
			p_entity,
			[ p_playerType, frameCount ]( System::TrajectoryPlayer & traj )
			{
				traj.playMode = p_playerType;
				switch ( p_playerType )
				{
				case System::TRAJECTORY_PLAY_MODE::PING_PONG:
					traj.player = Util::Players::PingPong( frameCount, traj.requestedFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::FORWARD:
					traj.player = Util::Players::Forward( frameCount, traj.requestedFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::FORWARD_LOOP:
					traj.player = Util::Players::ForwardLoop( frameCount, traj.requestedFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::BACKWARD:
					traj.player = Util::Players::Backward( frameCount, traj.requestedFrameIndex );
					break;
				case System::TRAJECTORY_PLAY_MODE::BACKWARD_LOOP:
					traj.player = Util::Players::BackwardLoop( frameCount, traj.requestedFrameIndex );
					break;
				default: traj.player = Util::Players::PingPong( frameCount, traj.requestedFrameIndex );
				}
			}
		);
	}

	void ChangeSpeed::execute( const Entity p_entity, const float p_speed )
	{
		if ( not std::isfinite( p_speed ) )
		{
			return;
		}

		const float speed = std::max( 1.f, p_speed );
		_patch( p_entity, [ speed ]( System::TrajectoryPlayer & traj ) { traj.playingSpeed = speed; } );
	}

} // namespace VTX::App::Action::Trajectory
