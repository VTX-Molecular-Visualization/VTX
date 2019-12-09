#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include <SDL2/SDL.h>

namespace VTX
{
	namespace Controller
	{
		class BaseController
		{
		  public:
			void handleEvent( const SDL_Event & );

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
