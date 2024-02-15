#include "app/core/trajectory_player/ping_pong.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	PingPong::PingPong( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory ) {}

	void PingPong::reset()
	{
		setCurrentFrameIndex( 0 );
		_forward = true;
	}

	void PingPong::nextFrame( const size_t p_frameCount )
	{
		size_t newFrameIndex = getCurrentFrameIndex();

		if ( _forward )
		{
			newFrameIndex += p_frameCount;
		}
		else
		{
			if ( newFrameIndex < p_frameCount )
			{
				newFrameIndex = p_frameCount - newFrameIndex;
				_forward	  = !_forward;
			}
			else
			{
				newFrameIndex -= p_frameCount;
			}
		}

		// Here forward always true
		if ( newFrameIndex >= getFrameCount() )
		{
			const size_t roundCount = newFrameIndex / getFrameCount();
			const size_t modulo		= newFrameIndex % ( getFrameCount() - 1 );

			_forward = ( roundCount % 2 == 0 );

			if ( _forward )
			{
				newFrameIndex = modulo;
			}
			else
			{
				newFrameIndex = ( getFrameCount() - 1 ) - modulo;
			}
		}

		setCurrentFrameIndex( newFrameIndex );
	}
} // namespace VTX::App::Core::TrajectoryPlayer
