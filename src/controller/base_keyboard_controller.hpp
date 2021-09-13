#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

#include "base_controller.hpp"
#include "event/base_event_receiver_keyboard.hpp"
#include "vtx_app.hpp"
#include <iostream>
#include <set>

namespace VTX
{
	namespace Controller
	{
		enum class KeyboardLayout : int
		{
			QWERTY,
			AZERTY
		};

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
					// if ( p_event.isAutoRepeat() == false )
					//{
					const Qt::Key key = Qt::Key( p_event.key() );
					_handleKeyDownEvent( key );
					//}
					// else
					//{
					//	_handleKeyPressedEvent( ScanCode( p_event.nativeScanCode() ) );
					//}
					break;
				}
				case QEvent::KeyRelease: _handleKeyUpEvent( Qt::Key( p_event.key() ) ); break;
				default: break;
				}
			}
			void clear() override { _pressedKeys.clear(); }

		  protected:
			static Qt::KeyboardModifiers keyboardModifiers() { return VTXApp::get().keyboardModifiers(); }
			static KeyboardLayout		 getKeyboardLayout()
			{
				switch ( VTXApp::get().inputMethod()->locale().language() )
				{
				case QLocale::France:
				case QLocale::French: return KeyboardLayout::AZERTY;
				default: return KeyboardLayout ::QWERTY;
				}
			}

			std::set<Qt::Key> _pressedKeys = std::set<Qt::Key>();

			virtual void _handleKeyDownEvent( const Qt::Key & p_key ) { _pressedKeys.emplace( p_key ); }
			virtual void _handleKeyUpEvent( const Qt::Key & p_key ) { _pressedKeys.erase( p_key ); }

			virtual void _handleKeyPressedEvent( const Qt::Key & p_key ) {}

			bool _isKeyPressed( const Qt::Key & p_key ) { return _pressedKeys.find( p_key ) != _pressedKeys.end(); }
		};
	} // namespace Controller
} // namespace VTX
#endif
