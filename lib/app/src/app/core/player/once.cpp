#include "app/core/player/once.hpp"

namespace VTX::App::Core::Player
{
	void Once::reset() { setCurrent( 0 ); }
	void Once::nextFrame( const size_t p_count )
	{
		const size_t newFrameIndex = getCurrent() + p_count;

		if ( newFrameIndex < getCount() - 1 )
		{
			setCurrent( newFrameIndex );
		}
		else
		{
			setCurrent( getCount() - 1 );
			pause();
		}
	}

} // namespace VTX::App::Core::Player