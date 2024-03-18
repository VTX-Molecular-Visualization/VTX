#include "app/core/trajectory_player/revert_loop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	void RevertLoop::reset() { setCurrent( getCount() - 1 ); }
	void RevertLoop::nextFrame( const size_t p_count )
	{
		if ( p_count > getCurrent() )
		{
			const size_t newFrameIndex = getCount() - ( ( p_count - getCurrent() ) % getCount() );
			setCurrent( newFrameIndex );
		}
		else
		{
			setCurrent( getCurrent() - p_count );
		}
	}

} // namespace VTX::App::Core::TrajectoryPlayer
