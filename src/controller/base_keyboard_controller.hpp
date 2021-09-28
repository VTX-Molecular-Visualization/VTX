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
			// STATICS ///////////////////////////////////////////////////////////////////
		  public:
			static void			  updateKeyboardBuffer( QKeyEvent & p_event );
			static void			  clear();
			static void			  clearKey( const Qt::Key & p_key );
			static KeyboardLayout getKeyboardLayout();

		  protected:
			static bool _isKeyPressed( const Qt::Key & p_key );
			static bool _isModifier( const ModifierFlag & p_modifier );
			static bool _isModifierExclusive( const ModifierFlag & p_modifier );

		  private:
			static ModifierFlag _getModifierFromKey( const Qt::Key & p_key );

			inline static std::set<Qt::Key> _pressedKeys = std::set<Qt::Key>();
			inline static ModifierFlag		_modifiers	 = ModifierFlag::None;
			//////////////////////////////////////////////////////////////////////////////

		  public:
			virtual void receiveEvent( const QKeyEvent & p_event ) override;

		  protected:
			virtual void _handleKeyDownEvent( const Qt::Key & p_key ) {};
			virtual void _handleKeyUpEvent( const Qt::Key & p_key ) {};
			// virtual void _handleKeyPressedEvent( const Qt::Key & p_key ) {};
		};
	} // namespace Controller
} // namespace VTX
#endif
