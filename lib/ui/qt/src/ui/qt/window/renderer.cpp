#include "ui/qt/window/renderer.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/services.hpp>
#include <array>
#include <util/event_hub.hpp>

namespace
{
	using Action	   = VTX::App::Input::InputManager::Action;
	using BindingTable = std::array<std::optional<Action>, 256>;

	const BindingTable _QWERTY_BINDINGS = []()
	{
		BindingTable bindings;
		bindings[ Qt::Key_W ] = Action::MoveFront;
		bindings[ Qt::Key_S ] = Action::MoveBack;
		bindings[ Qt::Key_A ] = Action::MoveLeft;
		bindings[ Qt::Key_D ] = Action::MoveRight;
		bindings[ Qt::Key_R ] = Action::MoveUp;
		bindings[ Qt::Key_F ] = Action::MoveDown;
		bindings[ Qt::Key_Q ] = Action::RotateLeft;
		bindings[ Qt::Key_E ] = Action::RotateRight;
		return bindings;
	}();

	const BindingTable _AZERTY_BINDINGS = []()
	{
		BindingTable bindings;
		bindings[ Qt::Key_Z ] = Action::MoveFront;
		bindings[ Qt::Key_S ] = Action::MoveBack;
		bindings[ Qt::Key_Q ] = Action::MoveLeft;
		bindings[ Qt::Key_D ] = Action::MoveRight;
		bindings[ Qt::Key_R ] = Action::MoveUp;
		bindings[ Qt::Key_F ] = Action::MoveDown;
		bindings[ Qt::Key_A ] = Action::RotateLeft;
		bindings[ Qt::Key_E ] = Action::RotateRight;
		return bindings;
	}();
} // namespace

namespace VTX::UI::QT::Window
{
	Renderer::Renderer() : QWindow(), _inputManager( App::INPUT() )
	{ App::HUB().connect<Events::KeyboardLayoutChanged, &Renderer::_onKBLayoutChange>( this ); }

	void Renderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void Renderer::focusInEvent( QFocusEvent * p_event )
	{
		QWindow::focusInEvent( p_event );
		_handleModifiers();
	}

	void Renderer::focusOutEvent( QFocusEvent * p_event )
	{
		QWindow::focusOutEvent( p_event );

		_inputManager.clearActions();
		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void Renderer::mousePressEvent( QMouseEvent * p_event )
	{
		_pressedButton = p_event->button();
		_pressPos	   = p_event->position();
		_lastPos	   = _pressPos;
		_dragging	   = false;
	}

	void Renderer::mouseMoveEvent( QMouseEvent * p_event )
	{
		if ( _pressedButton == Qt::NoButton )
		{
			return;
		}

		const int threshold = QGuiApplication::styleHints()->startDragDistance();
		if ( not _dragging && ( p_event->position() - _pressPos ).manhattanLength() >= threshold )
		{
			_dragging = true;
			_lastPos  = p_event->position();
			return;
		}

		if ( _dragging )
		{
			const QPoint delta = _toDevicePixels( p_event->position() - _lastPos );
			const Vec2i	 deltaVec( delta.x(), delta.y() );

			switch ( _pressedButton )
			{
			case Qt::LeftButton: _inputManager.rotateBy( deltaVec ); break;
			case Qt::MiddleButton: _inputManager.panBy( deltaVec ); break;
			case Qt::RightButton: _inputManager.rotateAltBy( deltaVec ); break;
			default: break;
			}

			_lastPos = p_event->position();
		}
	}

	void Renderer::mouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( p_event->button() != _pressedButton )
		{
			return;
		}

		if ( not _dragging )
		{
			emit clicked( _pressedButton, _toDevicePixels( p_event->position() ) );
		}

		_pressedButton = Qt::NoButton;
		_dragging	   = false;
	}

	void Renderer::mouseDoubleClickEvent( QMouseEvent * const )
	{
		_pressedButton = Qt::NoButton;
		_dragging	   = false;
		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void Renderer::wheelEvent( QWheelEvent * const p_event ) { _inputManager.zoomBy( p_event->angleDelta().y() ); }

	void Renderer::_handleKeyboard( QKeyEvent * const p_event, const bool p_enable )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		if ( const auto action = _getKeyboardAction( p_event->key() ); action.has_value() )
		{
			_inputManager.setAction( *action, p_enable );
		}

		_handleModifiers();
	}

	void Renderer::_handleModifiers()
	{
		const Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();
		_inputManager.setAction( App::Input::InputManager::Action::Accelerate, mods & Qt::ShiftModifier );
		_inputManager.setAction( App::Input::InputManager::Action::Decelerate, mods & Qt::AltModifier );
	}

	std::optional<App::Input::InputManager::Action> Renderer::_getKeyboardAction( const int p_key ) const
	{
		if ( p_key < 0 || p_key >= int( _QWERTY_BINDINGS.size() ) )
		{
			return std::nullopt;
		}

		const BindingTable & bindings = _layout == KB_LAYOUT::AZERTY ? _AZERTY_BINDINGS : _QWERTY_BINDINGS;
		return bindings[ p_key ];
	}

	QPoint Renderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatio();
		return { int( p_logicalPos.x() * dpr ), int( p_logicalPos.y() * dpr ) };
	}

	void Renderer::_onKBLayoutChange( const Events::KeyboardLayoutChanged & p_e )
	{ _layout = static_cast<KB_LAYOUT>( p_e.layout ); }

} // namespace VTX::UI::QT::Window
