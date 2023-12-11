#include "app/core/trajectory_player/revert_loop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	RevertLoop::RevertLoop( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory ) {}

	void RevertLoop::reset() { setCurrentFrameIndex( getFrameCount() - 1 ); }
	void RevertLoop::nextFrame( const size_t p_frameCount )
	{
		if ( p_frameCount > getCurrentFrameIndex() )
		{
			const size_t newFrameIndex
				= getFrameCount() - ( ( p_frameCount - getCurrentFrameIndex() ) % getFrameCount() );
			setCurrentFrameIndex( newFrameIndex );
		}
		else
		{
			setCurrentFrameIndex( getCurrentFrameIndex() - p_frameCount );
		}
	}

	std::unique_ptr<BasePlayer> RevertLoop::clone() const { return std::make_unique<RevertLoop>( *this ); }

} // namespace VTX::App::Core::TrajectoryPlayer
