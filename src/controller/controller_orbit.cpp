#include "controller_orbit.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerOrbit::_handleMouseButtonDownEvent( const SDL_MouseButtonEvent & p_event )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = true;
			default: break;
			}
		}

		void ControllerOrbit::_handleMouseButtonUpEvent( const SDL_MouseButtonEvent & p_event )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = false; break;
			case SDL_BUTTON_RIGHT: _mouseRightPressed = false; break;
			default: break;
			}
		}

		void ControllerOrbit::_handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event )
		{
			if ( _mouseLeftPressed )
			{
				// Rotate around.
			}
			else if ( _mouseRightPressed )
			{
				// Pan the target.
			}
		}

		void ControllerOrbit::_handleMouseWheelEvent( const SDL_MouseWheelEvent & p_event )
		{
			// Zoom
		}

	} // namespace Controller
} // namespace VTX
