#include "base_keyboard_mouse_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		void BaseKeyboardMouseController::handleEvent( const SDL_Event & p_event )
		{
			switch ( p_event.type )
			{
			case SDL_KEYDOWN: _handleKeyDownEvent( p_event.key.keysym.scancode ); break;
			case SDL_KEYUP: _handleKeyUpEvent( p_event.key.keysym.scancode ); break;
			case SDL_MOUSEBUTTONDOWN: _handleMouseButtonDownEvent( p_event.button ); break;
			case SDL_MOUSEBUTTONUP: _handleMouseButtonUpEvent( p_event.button ); break;
			case SDL_MOUSEMOTION: _handleMouseMotionEvent( p_event.motion ); break;
			case SDL_MOUSEWHEEL: _handleMouseWheelEvent( p_event.wheel );
			default: break;
			}
		}
	} // namespace Controller
} // namespace VTX
