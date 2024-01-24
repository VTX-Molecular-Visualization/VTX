#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/application_qt.hpp"
#include <app/vtx_app.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Input
{
	KeyboardLayout InputManager::getKeyboardLayout()
	{
		switch ( QT_APP()->inputMethod()->locale().language() )
		{
		case QLocale::Language::French: return KeyboardLayout::AZERTY;
		default: return KeyboardLayout ::QWERTY;
		}
	}

	Qt::Key InputManager::getKeyFromQwerty( const Qt::Key p_key )
	{
		Qt::Key res;

		switch ( getKeyboardLayout() )
		{
		case KeyboardLayout::AZERTY:
			switch ( p_key )
			{
			case Qt::Key::Key_W: res = Qt::Key::Key_Z; break;
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
		App::VTXApp::get().onPostUpdate().addCallback(
			this,
			[ this ]( const float p_deltaTime )
			{
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
		);
	}

	InputManager::~InputManager() {}

	void InputManager::handleKeyboardEvent( const QKeyEvent & p_event )
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

			onKeyPressed.call( key );

			break;
		}
		case QEvent::KeyRelease:
			_pressedKeys.erase( key );

			if ( modifier != ModifierFlag::None )
				_modifiers = ModifierFlag( _modifiers & !modifier );
			break;

			onKeyReleased.call( key );

		default: break;
		}
	}
	bool InputManager::isKeyPressed( const Qt::Key & p_key ) const
	{
		return _pressedKeys.find( p_key ) != _pressedKeys.end();
	}
	bool InputManager::isModifier( const ModifierFlag & p_modifier ) const { return ( _modifiers & p_modifier ) != 0; }
	bool InputManager::isModifierExclusive( const ModifierFlag & p_modifier ) const { return _modifiers == p_modifier; }
	void InputManager::clearKeyboardBuffer()
	{
		_pressedKeys.clear();
		_modifiers = ModifierFlag::None;
	}

	// Mouse
	void InputManager::handleMouseEvent( const QMouseEvent & p_event )
	{
		if ( _freeze )
			return;

		switch ( p_event.type() )
		{
		case QEvent::MouseButtonPress: _handleMouseButtonDownEvent( p_event ); break;
		case QEvent::MouseButtonRelease: _handleMouseButtonUpEvent( p_event ); break;
		case QEvent::MouseButtonDblClick: _handleMouseDoubleClickEvent( p_event ); break;
		case QEvent::MouseMove: _handleMouseMotionEvent( p_event ); break;
		default: break;
		}
	}

	void InputManager::handleMouseWheelEvent( const QWheelEvent & p_event )
	{
		if ( _freeze )
			return;

		switch ( p_event.type() )
		{
		case QEvent::Wheel: _handleMouseWheelEvent( p_event ); break;
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

	void InputManager::_handleMouseButtonDownEvent( const QMouseEvent & p_event )
	{
		// Save current position.
		_mousePosition.x = p_event.pos().x();
		_mousePosition.y = p_event.pos().y();

		switch ( p_event.button() )
		{
		case Qt::MouseButton::LeftButton:
			_mouseLeftPressed		= true;
			_isLeftClickCanceled	= false;
			_mouseLeftClickPosition = _mousePosition;
			if ( _mouseRightPressed )
			{
				_isRightClickCanceled = true;
			}
			break;
		case Qt::MouseButton::RightButton:
			_mouseRightPressed		 = true;
			_isRightClickCanceled	 = false;
			_mouseRightClickPosition = _mousePosition;
			if ( _mouseLeftPressed )
			{
				_isLeftClickCanceled = true;
			}
			break;
		case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = true; break;
		default: break;
		}
	}
	void InputManager::_handleMouseButtonUpEvent( const QMouseEvent & p_event )
	{
		switch ( p_event.button() )
		{
		case Qt::MouseButton::LeftButton:
			_mouseLeftPressed = false;
			if ( _isLeftClickCanceled == false )
			{
				onMouseLeftClicked.call( Vec2i( p_event.pos().x(), p_event.pos().y() ) );
			}
			break;
		case Qt::MouseButton::RightButton:
			_mouseRightPressed = false;
			if ( _isRightClickCanceled == false )
			{
				onMouseRightClicked.call( Vec2i( p_event.pos().x(), p_event.pos().y() ) );
			}
			break;
		case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = false; break;
		default: break;
		}
	}
	void InputManager::_handleMouseDoubleClickEvent( const QMouseEvent & p_event )
	{
		switch ( p_event.button() )
		{
		case Qt::MouseButton::LeftButton:
			onMouseLeftDoubleClicked.call( Vec2i( p_event.pos().x(), p_event.pos().y() ) );
			_isLeftClickCanceled = true;
			break;
		case Qt::MouseButton::RightButton: break;
		case Qt::MouseButton::MiddleButton: break;
		default: break;
		}
	}
	void InputManager::_handleMouseMotionEvent( const QMouseEvent & p_event )
	{
		Vec2i mousePosition = Vec2i();
		mousePosition.x		= p_event.pos().x();
		mousePosition.y		= p_event.pos().y();

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
	void InputManager::_handleMouseWheelEvent( const QWheelEvent & p_event )
	{
		// if ( p_event.modifiers().testFlag( Qt::AltModifier ) )
		if ( isModifier( ModifierFlag::Alt ) )
			_deltaMouseWheel = p_event.angleDelta().x();
		else
			_deltaMouseWheel = p_event.angleDelta().y();
	}

	void InputManager::freezeEvent( const bool p_freeze )
	{
		_freeze = p_freeze;
		clearKeyboardBuffer();
	}

	ModifierFlag InputManager::_getModifierFromKey( const Qt::Key & p_key )
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
	// void InputManager::clearKeyboardInputEvents() const { Controller::BaseKeyboardController::clear(); }
	// void InputManager::clearKeyboardInputEvent( const Qt::Key & p_key ) const
	//{
	//	Controller::BaseKeyboardController::clearKey( p_key );
	// }

} // namespace VTX::UI::QT::Input
