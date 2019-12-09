#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		void BaseState::handleEvent( const SDL_Event & p_event )
		{
			if ( _controller != nullptr ) { _controller->handleEvent( p_event ); }
		}

	} // namespace State
} // namespace VTX
