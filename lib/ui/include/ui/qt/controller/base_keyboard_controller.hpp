#ifndef __VTX_UI_QT_CONTROLLER_BASE_KEYBOARD_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_KEYBOARD_CONTROLLER__

#include "ui/core/controller/base_controller.hpp"
#include "ui/old_ui/event/base_event_receiver_keyboard.hpp"
#include <iostream>
#include <set>

namespace VTX::UI::QT::Controller
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

	class BaseKeyboardController :
		virtual public UI::Core::Controller::BaseController,
		public VTX::Event::BaseEventReceiverKeyboard
	{
	  public:
		virtual void receiveEvent( const QKeyEvent & p_event ) override;

		static void			  clear();
		static void			  clearKey( const Qt::Key & p_key );
		static KeyboardLayout getKeyboardLayout();
		static void			  updateKeyboardBuffer( const QKeyEvent & p_event );

	  protected:
		static bool _isKeyPressed( const Qt::Key & p_key );
		static bool _isModifier( const ModifierFlag & p_modifier );
		static bool _isModifierExclusive( const ModifierFlag & p_modifier );

	  private:
		inline static std::set<Qt::Key> _pressedKeys = std::set<Qt::Key>();
		inline static ModifierFlag		_modifiers	 = ModifierFlag::None;

		static ModifierFlag _getModifierFromKey( const Qt::Key & p_key );
	};
} // namespace VTX::UI::QT::Controller
#endif
