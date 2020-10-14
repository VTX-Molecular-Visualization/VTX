#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include "event/base_event_receiver_keyboard.hpp"
#include <iostream>
#include <set>

namespace VTX
{
	namespace Controller
	{
		class BaseKeyboardController : virtual public BaseController, public Event::BaseEventReceiverKeyboard
		{
		  public:
			virtual void receiveEvent( const QKeyEvent & p_event ) override
			{
				if ( isActive() == false )
					return;

				switch ( p_event.type() )
				{
				case QEvent::KeyPress:
				{
					// TOFIX: workaround beacause KeyRelease triggered after 1 second.
					/*if ( p_event.isAutoRepeat() == false )
					{*/
					_handleKeyDownEvent( p_event.key() );
					/*}
					else
					{
						_handleKeyPressedEvent( p_event.key() );
					}*/
					break;
				}
				case QEvent::KeyRelease: _handleKeyUpEvent( p_event.key() ); break;
				default: break;
				}
			}

		  protected:
			std::set<int> _pressedButtons = std::set<int>();

			virtual void _handleKeyDownEvent( const int & p_key ) { _pressedButtons.emplace( p_key ); }

			virtual void _handleKeyUpEvent( const int & p_key ) { _pressedButtons.erase( p_key ); }

			// virtual void _handleKeyPressedEvent( const int & p_key ) { std::cout << "_handleKeyPressedEvent" << std::endl; }

			bool _isKeyPressed( const int & p_key ) { return _pressedButtons.find( p_key ) != _pressedButtons.end(); }
		};
	} // namespace Controller
} // namespace VTX
#endif
