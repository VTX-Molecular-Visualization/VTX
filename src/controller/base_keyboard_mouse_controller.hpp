#ifndef __VTX_BASE_KEYBOARD_MOUSE_CONTROLLER__
#define __VTX_BASE_KEYBOARD_MOUSE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseKeyboardMouseController : public BaseController
		{
		  public:
			virtual void handleEvent( const SDL_Event & ) override;

		  protected:
			virtual void _handleKeyDownEvent( const SDL_Scancode & ) {};
			virtual void _handleKeyUpEvent( const SDL_Scancode & ) {};
			virtual void _handleMouseButtonDownEvent( const SDL_MouseButtonEvent & ) {};
			virtual void _handleMouseButtonUpEvent( const SDL_MouseButtonEvent & ) {};
			virtual void _handleMouseMotionEvent( const SDL_MouseMotionEvent & ) {};
			virtual void _handleMouseWheelEvent( const SDL_MouseWheelEvent & ) {};
		};
	} // namespace Controller
} // namespace VTX
#endif
