#include "app/core/input/input_manager.hpp"
#include <app/vtx_app.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Core::Input
{
	KeyboardLayout InputManager::getKeyboardLayout()
	{
		// TODO: Implement this function
		/*
		switch ( QT_APP()->inputMethod()->locale().language() )
		{
		case QLocale::Language::French: return KeyboardLayout::AZERTY;
		default: return KeyboardLayout ::QWERTY;
		}
		*/

		return KeyboardLayout::AZERTY;
	}

	Key InputManager::getKeyFromQwerty( const Key p_key )
	{
		Key res;

		switch ( getKeyboardLayout() )
		{
			// TODO: not sure this is needed if we use native keycodes?
		case KeyboardLayout::AZERTY:
			switch ( p_key )
			{
			case Key::Key_W: res = Key::Key_Z; break;
			case Key::Key_A: res = Key::Key_Q; break;
			case Key::Key_Q: res = Key::Key_A; break;
			case Key::Key_Z: res = Key::Key_W; break;
			case Key::Key_Semicolon: res = Key::Key_M; break;
			case Key::Key_Comma: res = Key::Key_Semicolon; break;
			case Key::Key_M: res = Key::Key_Comma; break;
			default: res = p_key; break;
			}
			break;
		case KeyboardLayout::QWERTY:
		default: res = p_key; break;
		}

		return res;
	}

	InputManager::InputManager()
	{
		// TODO: consume events, no auto clear.

		APP::onPostUpdate += [ this ]( const float p_deltaTime )
		{
			_deltaMousePosition.x = 0;
			_deltaMousePosition.y = 0;

			_deltaMouseWheel = 0;
		};
	}

	InputManager::~InputManager() {}

	void InputManager::handleKeyboardEvent( const KeyEvent & p_event )
	{
		if ( p_event.isRepeating )
			return;

		const Key		   key		= Key( p_event.key );
		const ModifierEnum modifier = _getModifierFromKey( key );

		switch ( p_event.type )
		{
		case KeyEvent::Type::Press:
		{
			_pressedKeys.emplace( key );

			if ( modifier != ModifierEnum::None )
				_modifiers |= modifier;

			onKeyPressed( key );

			break;
		}
		case KeyEvent::Type::Release:
		{
			_pressedKeys.erase( key );

			if ( modifier != ModifierEnum::None )
				_modifiers &= !modifier;

			onKeyReleased( key );
			break;
		}

		default: break;
		}
	}
	bool InputManager::isKeyPressed( const Key & p_key ) const
	{
		return _pressedKeys.find( p_key ) != _pressedKeys.end();
	}
	bool InputManager::isModifier( const ModifierFlag & p_modifier ) const
	{
		return uint( _modifiers & p_modifier ) != 0;
	}
	bool InputManager::isModifierExclusive( const ModifierFlag & p_modifier ) const { return _modifiers == p_modifier; }
	void InputManager::clearKeyboardBuffer()
	{
		_pressedKeys.clear();
		_modifiers = ModifierEnum::None;
	}

	// Mouse
	void InputManager::handleMouseEvent( const MouseEvent & p_event )
	{
		if ( _freeze )
			return;

		switch ( p_event.type )
		{
		case MouseEvent::Type::Press: _handleMouseButtonDownEvent( p_event ); break;
		case MouseEvent::Type::Release: _handleMouseButtonUpEvent( p_event ); break;
		case MouseEvent::Type::DoubleClick: _handleMouseDoubleClickEvent( p_event ); break;
		case MouseEvent::Type::Move: _handleMouseMotionEvent( p_event ); break;
		default: break;
		}
	}

	void InputManager::handleMouseWheelEvent( const WheelEvent & p_event )
	{
		if ( _freeze )
			return;

		switch ( p_event.type )
		{
		case WheelEvent::Type::Wheel: _handleMouseWheelEvent( p_event ); break;
		default: break;
		}
	}

	bool		  InputManager::isMouseLeftPressed() const { return _mouseLeftPressed; }
	bool		  InputManager::isMouseRightPressed() const { return _mouseRightPressed; }
	bool		  InputManager::isMouseMiddlePressed() const { return _mouseMiddlePressed; }
	const Vec2i & InputManager::getCurrentMousePosition() const { return _mousePosition; }
	const Vec2i & InputManager::getDeltaMousePosition() const { return _deltaMousePosition; }
	const Vec2i & InputManager::getMouseLeftClickPosition() const { return _mouseLeftClickPosition; }
	const Vec2i & InputManager::getMouseRightClickPosition() const { return _mouseRightClickPosition; }
	int			  InputManager::getDeltaMouseWheel() const { return _deltaMouseWheel; }

	void InputManager::_handleMouseButtonDownEvent( const MouseEvent & p_event )
	{
		// Save current position.
		_mousePosition.x = p_event.position.x;
		_mousePosition.y = p_event.position.y;

		switch ( p_event.button )
		{
		case MouseEvent::Button::Left:
			_mouseLeftPressed		= true;
			_isLeftClickCanceled	= false;
			_mouseLeftClickPosition = _mousePosition;
			if ( _mouseRightPressed )
			{
				_isRightClickCanceled = true;
			}
			break;
		case MouseEvent::Button::Right:
			_mouseRightPressed		 = true;
			_isRightClickCanceled	 = false;
			_mouseRightClickPosition = _mousePosition;
			if ( _mouseLeftPressed )
			{
				_isLeftClickCanceled = true;
			}
			break;
		case MouseEvent::Button::Middle: _mouseMiddlePressed = true; break;
		default: break;
		}
	}
	void InputManager::_handleMouseButtonUpEvent( const MouseEvent & p_event )
	{
		switch ( p_event.button )
		{
		case MouseEvent::Button::Left:
			_mouseLeftPressed = false;
			if ( _isLeftClickCanceled == false )
			{
				onMouseLeftClicked( Vec2i( p_event.position.x, p_event.position.y ) );
			}
			break;
		case MouseEvent::Button::Right:
			_mouseRightPressed = false;
			if ( _isRightClickCanceled == false )
			{
				onMouseRightClicked( Vec2i( p_event.position.x, p_event.position.y ) );
			}
			break;
		case MouseEvent::Button::Middle: _mouseMiddlePressed = false; break;
		default: break;
		}
	}
	void InputManager::_handleMouseDoubleClickEvent( const MouseEvent & p_event )
	{
		switch ( p_event.button )
		{
		case MouseEvent::Button::Left:
			onMouseLeftDoubleClicked( Vec2i( p_event.position.x, p_event.position.y ) );
			_isLeftClickCanceled = true;
			break;
		case MouseEvent::Button::Right: break;
		case MouseEvent::Button::Middle: break;
		default: break;
		}
	}
	void InputManager::_handleMouseMotionEvent( const MouseEvent & p_event )
	{
		Vec2i mousePosition = Vec2i();
		mousePosition.x		= p_event.position.x;
		mousePosition.y		= p_event.position.y;

		_deltaMousePosition += ( mousePosition - _mousePosition );
		_mousePosition = mousePosition;

		if ( _mouseLeftPressed )
		{
			// Mouse has moved too far from its start click pos => consider drag and not click
			if ( Util::Math::distance( Vec2f( mousePosition ), Vec2f( _mouseLeftClickPosition ) ) > CLICK_MAX_DISTANCE )
			{
				_isLeftClickCanceled = true;
			}
		}

		if ( _mouseRightPressed )
		{
			// Mouse has moved too far from its start click pos => consider drag and not click
			if ( Util::Math::distance( Vec2f( mousePosition ), Vec2f( _mouseRightClickPosition ) )
				 > CLICK_MAX_DISTANCE )
			{
				_isRightClickCanceled = true;
			}
		}
	}
	void InputManager::_handleMouseWheelEvent( const WheelEvent & p_event )
	{
		if ( isModifier( ModifierEnum::Alt ) )
			_deltaMouseWheel = p_event.angleDelta.x;
		else
			_deltaMouseWheel = p_event.angleDelta.y;
	}

	void InputManager::freezeEvent( const bool p_freeze )
	{
		_freeze = p_freeze;
		clearKeyboardBuffer();
	}

	ModifierEnum InputManager::_getModifierFromKey( const Key & p_key )
	{
		ModifierEnum modifier;

		switch ( p_key )
		{
		case Key::Key_Control: modifier = ModifierEnum::Ctrl; break;
		case Key::Key_Shift: modifier = ModifierEnum::Shift; break;
		case Key::Key_Alt: modifier = ModifierEnum::Alt; break;
		case Key::Key_AltGr: modifier = ModifierEnum::AltGr; break;
		default: modifier = ModifierEnum::None; break;
		}

		return modifier;
	}
	// void InputManager::clearKeyboardInputEvents() const { Controller::BaseKeyboardController::clear(); }
	// void InputManager::clearKeyboardInputEvent( const Key & p_key ) const
	//{
	//	Controller::BaseKeyboardController::clearKey( p_key );
	// }

} // namespace VTX::App::Core::Input
