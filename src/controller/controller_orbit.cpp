#include "controller_orbit.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerOrbit::_handleMouseButtonDownEvent( const SDL_MouseButtonEvent & p_event,
														   const double					p_deltaTime )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = true; break;
			case SDL_BUTTON_RIGHT: _mouseRightPressed = true; break;
			default: break;
			}
		}

		void ControllerOrbit::_handleMouseButtonUpEvent( const SDL_MouseButtonEvent & p_event,
														 const double				  p_deltaTime )
		{
			switch ( p_event.button )
			{
			case SDL_BUTTON_LEFT: _mouseLeftPressed = false; break;
			case SDL_BUTTON_RIGHT: _mouseRightPressed = false; break;
			default: break;
			}
		}

		void ControllerOrbit::_handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event, const double p_deltaTime )
		{
			if ( _mouseLeftPressed )
			{
				// Rotate around.
				//_camera.rotateAroundLeft( 0.1f * p_event.xrel, _target );
				//_camera.rotateAroundUp( 0.1f * p_event.yrel, _target );
			}
			else if ( _mouseRightPressed )
			{
				// Pan the target?
			}
		}

		void ControllerOrbit::_handleMouseWheelEvent( const SDL_MouseWheelEvent & p_event, const double p_deltaTime )
		{
			// Zoom
		}

	} // namespace Controller
} // namespace VTX
