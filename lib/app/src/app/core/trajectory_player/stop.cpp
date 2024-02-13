#include "app/core/trajectory_player/stop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	Stop::Stop() : BasePlayer() { pause(); }
	Stop::Stop( const Stop & p_source ) : BasePlayer( p_source ) { pause(); }
	Stop::Stop( VTX::Core::Struct::Trajectory * const p_trajectory ) : BasePlayer( p_trajectory ) { pause(); }

	void Stop::play() {}

	void Stop::reset()
	{
		if ( isLinkedToTrajectory() )
			setCurrentFrameIndex( 0 );
	}
	void Stop::nextFrame( const size_t p_frameCount ) {}

} // namespace VTX::App::Core::TrajectoryPlayer
