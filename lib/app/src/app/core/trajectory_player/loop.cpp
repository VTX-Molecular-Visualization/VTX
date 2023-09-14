#include "app/core/trajectory_player/loop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	Loop::Loop( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory )
	{
		_displayName = "Loop";
	}

	void Loop::reset() { setCurrentFrameIndex( 0 ); }
	void Loop::nextFrame( const size_t p_frameCount )
	{
		const size_t newFrameIndex = ( getCurrentFrameIndex() + p_frameCount ) % getFrameCount();
		setCurrentFrameIndex( newFrameIndex );
	}
} // namespace VTX::App::Core::TrajectoryPlayer
