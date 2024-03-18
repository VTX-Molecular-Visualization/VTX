#include "app/core/trajectory_player/loop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	void Loop::reset() { setCurrent( 0 ); }
	void Loop::nextFrame( const size_t p_count )
	{
		const size_t newFrameIndex = ( getCurrent() + p_count ) % getCount();
		setCurrent( newFrameIndex );
	}

} // namespace VTX::App::Core::TrajectoryPlayer
