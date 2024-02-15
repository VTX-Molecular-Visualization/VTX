#include "app/core/trajectory_player/revert_once.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	RevertOnce::RevertOnce( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory ) {}

	void RevertOnce::reset() { setCurrentFrameIndex( getFrameCount() - 1 ); }
	void RevertOnce::nextFrame( const size_t p_frameCount )
	{
		if ( p_frameCount >= getCurrentFrameIndex() )
		{
			setCurrentFrameIndex( 0 );
			pause();
		}
		else
		{
			setCurrentFrameIndex( getCurrentFrameIndex() - p_frameCount );
		}
	}

} // namespace VTX::App::Core::TrajectoryPlayer
