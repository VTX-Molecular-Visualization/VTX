#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include <set>

namespace VTX
{
	namespace Controller
	{
		class BaseKeyboardController : virtual public BaseController
		{
			/*
		  public:
			virtual void receiveEvent( const SDL_Event & p_event ) override
			{
				switch ( p_event.type )
				{
				case SDL_KEYDOWN:
				{
					_handleKeyDownEvent( p_event.key.keysym.scancode );
					if ( p_event.key.repeat == 0 ) { _handleKeyPressedEvent( p_event.key.keysym.scancode ); }
					break;
				}
				case SDL_KEYUP: _handleKeyUpEvent( p_event.key.keysym.scancode ); break;
				default: break;
				}
			}

		  protected:
			std::set<SDL_Scancode> _pressedButtons = std::set<SDL_Scancode>();

			virtual void _handleKeyDownEvent( const SDL_Scancode & p_key ) { _pressedButtons.emplace( p_key ); };
			virtual void _handleKeyUpEvent( const SDL_Scancode & p_key ) { _pressedButtons.erase( p_key ); };
			virtual void _handleKeyPressedEvent( const SDL_Scancode & p_key ) {}

			bool _isKeyPressed( const SDL_Scancode & p_key )
			{
				return _pressedButtons.find( p_key ) != _pressedButtons.end();
			}
			*/
		};
	} // namespace Controller
} // namespace VTX
#endif
