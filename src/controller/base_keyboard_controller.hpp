#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseKeyboardController : virtual public BaseController
		{
		  public:
			virtual void handleEvent( const SDL_Event & p_event, void * const ) override
			{
				switch ( p_event.type )
				{
				case SDL_KEYDOWN: _handleKeyDownEvent( p_event.key.keysym.scancode ); break;
				case SDL_KEYUP: _handleKeyUpEvent( p_event.key.keysym.scancode ); break;
				default: break;
				}
			}

		  protected:
			virtual void _handleKeyDownEvent( const SDL_Scancode & ) {};
			virtual void _handleKeyUpEvent( const SDL_Scancode & ) {};
		};
	} // namespace Controller
} // namespace VTX
#endif
