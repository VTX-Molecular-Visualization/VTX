#include "base_keyboard_controller.hpp"
#include "vtx_app.hpp"

namespace VTX::Controller
{
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

	KeyboardLayout BaseKeyboardController::getKeyboardLayout()
	{
		switch ( VTXApp::get().inputMethod()->locale().language() )
		{
		case QLocale::France:
		case QLocale::French: return KeyboardLayout::AZERTY;
		default: return KeyboardLayout ::QWERTY;
		}
	}

	void BaseKeyboardController::_handleKeyDownEvent( const Qt::Key & p_key )
	{
		_pressedKeys.emplace( p_key );

		switch ( p_key )
		{
		case Qt::Key::Key_Control: _modifiers = ModifierFlag( _modifiers | ModifierFlag::Control ); break;
		case Qt::Key::Key_Shift: _modifiers = ModifierFlag( _modifiers | ModifierFlag::Shift ); break;
		case Qt::Key::Key_Alt: _modifiers = ModifierFlag( _modifiers | ModifierFlag::Alt ); break;
		case Qt::Key::Key_AltGr: _modifiers = ModifierFlag( _modifiers | ModifierFlag::AltGr ); break;

		default: break;
		}
	}

	void BaseKeyboardController::_handleKeyUpEvent( const Qt::Key & p_key )
	{
		_pressedKeys.erase( p_key );

		switch ( p_key )
		{
		case Qt::Key::Key_Control: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::Control ); break;
		case Qt::Key::Key_Shift: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::Shift ); break;
		case Qt::Key::Key_Alt: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::Alt ); break;
		case Qt::Key::Key_AltGr: _modifiers = ModifierFlag( _modifiers & !ModifierFlag::AltGr ); break;

		default: break;
		}
	}

	bool BaseKeyboardController::_getModifier( const ModifierFlag & p_modifier ) { return _modifiers & p_modifier; }

	bool BaseKeyboardController::_getExclusiveModifier( const ModifierFlag & p_modifier )
	{
		return _getModifier( p_modifier ) && ( !_modifiers & p_modifier ) == 0;
	}

} // namespace VTX::Controller
