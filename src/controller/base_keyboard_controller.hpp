#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

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
			// ScanCode from AZERTY layout.
			enum class ScanCode : uint
			{
				F1			= 59,
				F2			= 60,
				F3			= 61,
				F4			= 62,
				F5			= 63,
				F6			= 64,
				F7			= 65,
				F8			= 66,
				F9			= 67,
				F10			= 68,
				F11			= 87,
				Z			= 17,
				S			= 31,
				Q			= 30,
				D			= 32,
				A			= 16,
				E			= 18,
				R			= 19,
				F			= 33,
				Left		= 331,
				Right		= 333,
				Up			= 328,
				Down		= 336,
				LeftControl = 29,
				LeftShift	= 42,
				Delete		= 339,
			};

			virtual void receiveEvent( const QKeyEvent & p_event ) override
			{
				if ( isActive() == false )
					return;

				switch ( p_event.type() )
				{
				case QEvent::KeyPress:
				{
					// TOFIX: workaround beacause KeyRelease triggered after 1 second.
					// if ( p_event.isAutoRepeat() == false )
					//{
					const quint32 nativeCode = p_event.nativeScanCode();
					_handleKeyDownEvent( ScanCode( nativeCode ) );
					//}
					// else
					//{
					//	_handleKeyPressedEvent( ScanCode( p_event.nativeScanCode() ) );
					//}
					break;
				}
				case QEvent::KeyRelease: _handleKeyUpEvent( ScanCode( p_event.nativeScanCode() ) ); break;
				default: break;
				}
			}
			void clear() override { _pressedKeys.clear(); }

		  protected:
			std::set<ScanCode> _pressedKeys = std::set<ScanCode>();

			virtual void _handleKeyDownEvent( const ScanCode & p_key ) { _pressedKeys.emplace( p_key ); }
			virtual void _handleKeyUpEvent( const ScanCode & p_key ) { _pressedKeys.erase( p_key ); }

			virtual void _handleKeyPressedEvent( const ScanCode & p_key ) {}

			bool _isKeyPressed( const ScanCode & p_key ) { return _pressedKeys.find( p_key ) != _pressedKeys.end(); }
		};
	} // namespace Controller
} // namespace VTX
#endif
