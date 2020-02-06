#include "controller_shortcut.hpp"
#include "../define.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerShortcut::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_F10 ) { VTX_DEBUG( "F10" ); }
		}

	} // namespace Controller
} // namespace VTX
