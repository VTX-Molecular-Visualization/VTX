#include "app/core/trajectory_player/stop.hpp"

namespace VTX::App::Core::TrajectoryPlayer
{
	Stop::Stop() : BasePlayer() { pause(); }
	Stop::Stop( const Stop & p_source ) : BasePlayer( p_source ) { pause(); }

	void Stop::play() {}

	void Stop::reset() { setCurrent( 0 ); }
	void Stop::nextFrame( const size_t p_frameCount ) {}

} // namespace VTX::App::Core::TrajectoryPlayer
