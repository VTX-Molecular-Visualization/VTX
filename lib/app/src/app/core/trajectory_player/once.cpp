#include "app/core/trajectory_player/once.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	Once::Once( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory ) {}

	void Once::reset() { setCurrentFrameIndex( 0 ); }
	void Once::nextFrame( const size_t p_frameCount )
	{
		const size_t newFrameIndex = getCurrentFrameIndex() + p_frameCount;

		if ( newFrameIndex < getFrameCount() - 1 )
		{
			setCurrentFrameIndex( newFrameIndex );
		}
		else
		{
			setCurrentFrameIndex( getFrameCount() - 1 );
			pause();
		}
	}
} // namespace VTX::App::Core::TrajectoryPlayer
