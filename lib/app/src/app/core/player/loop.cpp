#include "app/core/player/loop.hpp"
#include <util/logger.hpp>

namespace VTX::App::Core::Player
{
	void Loop::reset() { setCurrent( 0 ); }
	void Loop::nextFrame( const size_t p_count )
	{
		const size_t newFrameIndex = ( getCurrent() + p_count ) % getCount();
		setCurrent( newFrameIndex );
	}

} // namespace VTX::App::Core::Player
