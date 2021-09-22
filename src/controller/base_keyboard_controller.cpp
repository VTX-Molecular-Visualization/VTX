#include "base_keyboard_controller.hpp"

namespace VTX::Controller
{
	BaseKeyboardController::BaseKeyboardController() {}

	void BaseKeyboardController::receiveEvent( const QKeyEvent & p_event )
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

	void BaseKeyboardController::_handleKeyDownEvent( const ScanCode & p_key )
	{
		_pressedKeys.emplace( p_key );

		switch ( p_key )
		{
		case ScanCode::LeftControl: _modifiers = ModifierFlag( _modifiers | ModifierFlag::LeftControl ); break;
		case ScanCode::RightControl: _modifiers = ModifierFlag( _modifiers | ModifierFlag::RightControl ); break;
		case ScanCode::LeftShift: _modifiers = ModifierFlag( _modifiers | ModifierFlag::LeftShift ); break;
		case ScanCode::RightShift: _modifiers = ModifierFlag( _modifiers | ModifierFlag::RightShift ); break;
		case ScanCode::Alt:
			_modifiers = ModifierFlag( _modifiers | ModifierFlag::Alt );
			break;
			// case ScanCode::AltGr: ModifierFlag( _modifiers | ModifierFlag::AltGr ); break;

		default: break;
		}
	}
	void BaseKeyboardController::_handleKeyUpEvent( const ScanCode & p_key )
	{
		_pressedKeys.erase( p_key );

		switch ( p_key )
		{
		case ScanCode::LeftControl: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::LeftControl ); break;
		case ScanCode::RightControl: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::RightControl ); break;
		case ScanCode::LeftShift: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::LeftShift ); break;
		case ScanCode::RightShift: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::RightShift ); break;
		case ScanCode::Alt:
			_modifiers = ModifierFlag( _modifiers & !ModifierFlag::Alt );
			break;
			// case ScanCode::AltGr: ModifierFlag( _modifiers & !ModifierFlag::AltGr ); break;

		default: break;
		}
	}

	bool BaseKeyboardController::_getModifier( const ModifierFlag & p_modifier ) { return _modifiers & p_modifier; }

	bool BaseKeyboardController::_getExclusiveModifier( const ModifierFlag & p_modifier )
	{
		return _getModifier( p_modifier ) && ( !_modifiers & p_modifier ) == 0;
	}

} // namespace VTX::Controller
