#include "base_keyboard_controller.hpp"
#include "qt/application_qt.hpp"
#include <QLocale>

namespace VTX::UI::QT::Controller
{
	void BaseKeyboardController::updateKeyboardBuffer( const QKeyEvent & p_event )
	{
		if ( p_event.isAutoRepeat() )
			return;

		const Qt::Key	   key		= Qt::Key( p_event.key() );
		const ModifierFlag modifier = _getModifierFromKey( key );

		switch ( p_event.type() )
		{
		case QEvent::KeyPress:
		{
			_pressedKeys.emplace( key );
			if ( modifier != ModifierFlag::None )
				_modifiers = ModifierFlag( _modifiers | modifier );
			break;
		}
		case QEvent::KeyRelease:
			_pressedKeys.erase( key );

			if ( modifier != ModifierFlag::None )
				_modifiers = ModifierFlag( _modifiers & !modifier );
			break;
		default: break;
		}
	}

	void BaseKeyboardController::clear()
	{
		_pressedKeys.clear();
		_modifiers = ModifierFlag::None;
	}
	void BaseKeyboardController::clearKey( const Qt::Key & p_key )
	{
		_pressedKeys.erase( p_key );

		ModifierFlag modifier = _getModifierFromKey( p_key );

		if ( modifier != ModifierFlag::None )
			_modifiers = ModifierFlag( _modifiers & !modifier );
	}

	KeyboardLayout BaseKeyboardController::getKeyboardLayout()
	{
		switch ( QT_APP()->inputMethod()->locale().language() )
		{
		case QLocale::Language::French: return KeyboardLayout::AZERTY;
		default: return KeyboardLayout ::QWERTY;
		}
	}
	bool BaseKeyboardController::_isKeyPressed( const Qt::Key & p_key )
	{
		return _pressedKeys.find( p_key ) != _pressedKeys.end();
	}
	bool BaseKeyboardController::_isModifier( const ModifierFlag & p_modifier )
	{
		return ( _modifiers & p_modifier ) != 0;
	}
	bool BaseKeyboardController::_isModifierExclusive( const ModifierFlag & p_modifier )
	{
		return _modifiers == p_modifier;
	}

	ModifierFlag BaseKeyboardController::_getModifierFromKey( const Qt::Key & p_key )
	{
		ModifierFlag modifier;

		switch ( p_key )
		{
		case Qt::Key::Key_Control: modifier = ModifierFlag::Control; break;
		case Qt::Key::Key_Shift: modifier = ModifierFlag::Shift; break;
		case Qt::Key::Key_Alt: modifier = ModifierFlag::Alt; break;
		case Qt::Key::Key_AltGr: modifier = ModifierFlag::AltGr; break;
		default: modifier = ModifierFlag::None; break;
		}

		return modifier;
	}

	void BaseKeyboardController::receiveEvent( const QKeyEvent & p_event ) {}

} // namespace VTX::UI::QT::Controller
