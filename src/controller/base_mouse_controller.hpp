#ifndef __VTX_BASE_MOUSE_CONTROLLER__
#define __VTX_BASE_MOUSE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include <set>

namespace VTX
{
	namespace Controller
	{
		class BaseMouseController : virtual public BaseController
		{
			/*
		  public:
			virtual void receiveEvent( const SDL_Event & p_event ) override
			{
				switch ( p_event.type )
				{
				case SDL_MOUSEBUTTONDOWN: _handleMouseButtonDownEvent( p_event.button ); break;
				case SDL_MOUSEBUTTONUP: _handleMouseButtonUpEvent( p_event.button ); break;
				case SDL_MOUSEMOTION: _handleMouseMotionEvent( p_event.motion ); break;
				case SDL_MOUSEWHEEL: _handleMouseWheelEvent( p_event.wheel );
				default: break;
				}
			}

		  protected:
			bool  _mouseLeftPressed	  = false;
			bool  _mouseRightPressed  = false;
			bool  _mouseMiddlePressed = false;
			Vec2i _deltaMousePosition = Vec2i();
			int	  _deltaMouseWheel	  = 0;

			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent & p_event )
			{
				switch ( p_event.button )
				{
				case SDL_BUTTON_LEFT: _mouseLeftPressed = true; break;
				case SDL_BUTTON_RIGHT: _mouseRightPressed = true; break;
				case SDL_BUTTON_MIDDLE: _mouseMiddlePressed = true; break;
				default: break;
				}
			}

			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent & p_event )
			{
				switch ( p_event.button )
				{
				case SDL_BUTTON_LEFT: _mouseLeftPressed = false; break;
				case SDL_BUTTON_RIGHT: _mouseRightPressed = false; break;
				case SDL_BUTTON_MIDDLE: _mouseMiddlePressed = false; break;
				default: break;
				}
			}

			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event )
			{
				_deltaMousePosition.x = p_event.xrel;
				_deltaMousePosition.y = p_event.yrel;
			}
			virtual void _handleMouseWheelEvent( const SDL_MouseWheelEvent & p_event )
			{
				_deltaMouseWheel = p_event.y;
			}
			*/
		};
	} // namespace Controller
} // namespace VTX
#endif
