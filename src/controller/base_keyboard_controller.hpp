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

		enum ModifierFlag : uint
		{
			None	= 0,
			Control = 1 << 0,
			Shift	= 1 << 1,
			Alt		= 1 << 2,
			AltGr	= 1 << 3,
		};

		class BaseKeyboardController : virtual public BaseController, public Event::BaseEventReceiverKeyboard
		{
		  public:
			virtual void receiveEvent( const QKeyEvent & p_event ) override;
			void		 clear() override
			{
				_pressedKeys.clear();
				_modifiers = ModifierFlag::None;
			}

		  protected:
			static Qt::KeyboardModifiers keyboardModifiers() { return VTXApp::get().keyboardModifiers(); }
			static KeyboardLayout		 getKeyboardLayout();

			std::set<Qt::Key> _pressedKeys = std::set<Qt::Key>();
			ModifierFlag	  _modifiers   = ModifierFlag::None;

			virtual void _handleKeyDownEvent( const Qt::Key & p_key );
			virtual void _handleKeyUpEvent( const Qt::Key & p_key );

			virtual void _handleKeyPressedEvent( const Qt::Key & p_key ) {}

			bool _isKeyPressed( const Qt::Key & p_key ) { return _pressedKeys.find( p_key ) != _pressedKeys.end(); }
			bool _getModifier( const ModifierFlag & p_modifier );
			bool _getExclusiveModifier( const ModifierFlag & p_modifier );
		};
	} // namespace Controller
} // namespace VTX
#endif
