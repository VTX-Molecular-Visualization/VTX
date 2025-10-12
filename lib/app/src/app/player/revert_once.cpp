#include "app/player/revert_once.hpp"

namespace VTX::App::Player
{
	void RevertOnce::reset() { setCurrent( getCount() - 1 ); }
	void RevertOnce::nextFrame( const size_t p_frameCount )
	{
		if ( p_frameCount >= getCurrent() )
		{
			setCurrent( 0 );
			pause();
		}
		else
		{
			setCurrent( getCurrent() - p_frameCount );
		}
	}

} // namespace VTX::App::Player
