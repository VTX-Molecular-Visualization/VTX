#include "app/core/trajectory_player/ping_pong.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	void PingPong::reset()
	{
		setCurrent( 0 );
		_forward = true;
	}

	void PingPong::nextFrame( const size_t p_count )
	{
		size_t newFrameIndex = getCurrent();

		if ( _forward )
		{
			newFrameIndex += p_count;
		}
		else
		{
			if ( newFrameIndex < p_count )
			{
				newFrameIndex = p_count - newFrameIndex;
				_forward	  = !_forward;
			}
			else
			{
				newFrameIndex -= p_count;
			}
		}

		// Here forward always true
		if ( newFrameIndex >= getCount() )
		{
			const size_t roundCount = newFrameIndex / getCount();
			const size_t modulo		= newFrameIndex % ( getCount() - 1 );

			_forward = ( roundCount % 2 == 0 );

			if ( _forward )
			{
				newFrameIndex = modulo;
			}
			else
			{
				newFrameIndex = ( getCount() - 1 ) - modulo;
			}
		}

		setCurrent( newFrameIndex );
	}
} // namespace VTX::App::Core::TrajectoryPlayer
