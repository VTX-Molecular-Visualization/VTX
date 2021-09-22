#ifndef __VTX_BASE_KEYBOARD_CONTROLLER__
#define __VTX_BASE_KEYBOARD_CONTROLLER__

#include "base_controller.hpp"
#include "event/base_event_receiver_keyboard.hpp"
#include <iostream>
#include <set>
#include <vector>

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
				Escape		 = 1,
				F1			 = 59,
				F2			 = 60,
				F3			 = 61,
				F4			 = 62,
				F5			 = 63,
				F6			 = 64,
				F7			 = 65,
				F8			 = 66,
				F9			 = 67,
				F10			 = 68,
				F11			 = 87,
				Z			 = 17,
				S			 = 31,
				Q			 = 30,
				D			 = 32,
				A			 = 16,
				E			 = 18,
				R			 = 19,
				F			 = 33,
				O			 = 24,
				N			 = 49,
				Left		 = 331,
				Right		 = 333,
				Up			 = 328,
				Down		 = 336,
				LeftControl	 = 29,
				RightControl = 285,
				LeftShift	 = 42,
				RightShift	 = 54,
				Alt			 = 56,
				// AltGr		 = 29,
				Delete = 339,
			};

			enum ModifierFlag : uint
			{
				None		 = 0,
				LeftControl	 = 1 << 0,
				RightControl = 1 << 1,
				LeftShift	 = 1 << 2,
				RightShift	 = 1 << 3,
				Alt			 = 1 << 4,
				AltGr		 = 1 << 5,

				Shift	= LeftShift | RightShift,
				Control = LeftControl | RightControl
			};

			BaseKeyboardController();

			virtual void receiveEvent( const QKeyEvent & p_event ) override;
			void		 clear() override
			{
				_pressedKeys.clear();
				_modifiers = ModifierFlag::None;
			}

		  protected:
			std::set<ScanCode> _pressedKeys = std::set<ScanCode>();
			ModifierFlag	   _modifiers	= ModifierFlag::None;

			virtual void _handleKeyDownEvent( const ScanCode & p_key );
			virtual void _handleKeyUpEvent( const ScanCode & p_key );

			virtual void _handleKeyPressedEvent( const ScanCode & p_key ) {}

			bool _isKeyPressed( const ScanCode & p_key ) { return _pressedKeys.find( p_key ) != _pressedKeys.end(); }
			bool _getModifier( const ModifierFlag & p_modifier );
			bool _getExclusiveModifier( const ModifierFlag & p_modifier );
		};
	} // namespace Controller
} // namespace VTX
#endif
