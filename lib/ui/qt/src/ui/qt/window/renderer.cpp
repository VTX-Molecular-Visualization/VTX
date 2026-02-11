#include "ui/qt/window/renderer.hpp"
#include "ui/qt/helper.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>
#include <util/type_traits.hpp>

namespace
{
	using namespace VTX::UI::QT::Window;

	/**
	 * @brief Available actions from keyboard.
	 */
	enum ACTION
	{
		MOVE_FRONT,	 // W
		MOVE_BACK,	 // S
		MOVE_LEFT,	 // A
		MOVE_RIGHT,	 // D
		MOVE_UP,	 // R
		MOVE_DOWN,	 // F
		ROTATE_LEFT, // Q
		ROTATE_RIGHT // E
	};

	/**
	 * @brief Get the key from action and layout.
	 */
	template<ACTION A, KB_LAYOUT = KB_LAYOUT::QWERTY>
	constexpr Qt::Key _key()
	{
		if constexpr ( A == MOVE_FRONT )
		{
			return Qt::Key::Key_W;
		}
		else if constexpr ( A == MOVE_BACK )
		{
			return Qt::Key::Key_S;
		}
		else if constexpr ( A == MOVE_LEFT )
		{
			return Qt::Key::Key_A;
		}
		else if constexpr ( A == MOVE_RIGHT )
		{
			return Qt::Key::Key_D;
		}
		else if constexpr ( A == MOVE_UP )
		{
			return Qt::Key::Key_R;
		}
		else if constexpr ( A == MOVE_DOWN )
		{
			return Qt::Key::Key_F;
		}
		else if constexpr ( A == ROTATE_LEFT )
		{
			return Qt::Key::Key_Q;
		}
		else if constexpr ( A == ROTATE_RIGHT )
		{
			return Qt::Key::Key_E;
		}
		else
		{
			static_assert( VTX::always_false_v<A>, "Invalid action" );
			return Qt::Key::Key_unknown;
		}
	}

	template<>
	constexpr Qt::Key _key<MOVE_FRONT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_Z;
	}

	template<>
	constexpr Qt::Key _key<MOVE_LEFT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_Q;
	}

	template<>
	constexpr Qt::Key _key<ROTATE_LEFT, KB_LAYOUT::AZERTY>()
	{
		return Qt::Key::Key_A;
	}

} // namespace

namespace VTX::UI::QT::Window
{

	Renderer::Renderer() : QWindow(), _inputManager( App::INPUT() )
	{
		App::HUB().connect<Events::KeyboardLayoutChanged, &Renderer::_onKBLayoutChange>( this );
	}

	void Renderer::keyPressEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, true ); }

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event ) { _handleKeyboard( p_event, false ); }

	void Renderer::_handleKeyboard( QKeyEvent * const p_event, const bool p_enable )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		if ( _layout == KB_LAYOUT::QWERTY )
		{
			switch ( p_event->key() )
			{
			case _key<MOVE_FRONT>(): _inputManager.setMoveFront( p_enable ); break;
			case _key<MOVE_BACK>(): _inputManager.setMoveBack( p_enable ); break;
			case _key<MOVE_LEFT>(): _inputManager.setMoveLeft( p_enable ); break;
			case _key<MOVE_RIGHT>(): _inputManager.setMoveRight( p_enable ); break;
			case _key<MOVE_UP>(): _inputManager.setMoveUp( p_enable ); break;
			case _key<MOVE_DOWN>(): _inputManager.setMoveDown( p_enable ); break;
			case _key<ROTATE_LEFT>(): _inputManager.setRotateLeft( p_enable ); break;
			case _key<ROTATE_RIGHT>(): _inputManager.setRotateRight( p_enable ); break;
			default: break;
			}
		}
		else if ( _layout == KB_LAYOUT::AZERTY )
		{
			switch ( p_event->key() )
			{
			case _key<MOVE_FRONT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveFront( p_enable ); break;
			case _key<MOVE_BACK, KB_LAYOUT::AZERTY>(): _inputManager.setMoveBack( p_enable ); break;
			case _key<MOVE_LEFT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveLeft( p_enable ); break;
			case _key<MOVE_RIGHT, KB_LAYOUT::AZERTY>(): _inputManager.setMoveRight( p_enable ); break;
			case _key<MOVE_UP, KB_LAYOUT::AZERTY>(): _inputManager.setMoveUp( p_enable ); break;
			case _key<MOVE_DOWN, KB_LAYOUT::AZERTY>(): _inputManager.setMoveDown( p_enable ); break;
			case _key<ROTATE_LEFT, KB_LAYOUT::AZERTY>(): _inputManager.setRotateLeft( p_enable ); break;
			case _key<ROTATE_RIGHT, KB_LAYOUT::AZERTY>(): _inputManager.setRotateRight( p_enable ); break;
			default: break;
			}
		}

		_handleModifiers();
	}

	void Renderer::_handleModifiers()
	{
		const Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();

		_inputManager.setAccelerate( mods & Qt::ShiftModifier );
		_inputManager.setDecelerate( mods & Qt::ControlModifier );
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
			QPoint delta = _toDevicePixels( p_event->position() - _lastPos );
			Vec2i  deltaVec( delta.x(), delta.y() );

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

	void Renderer::mouseDoubleClickEvent( QMouseEvent * const p_event )
	{
		_pressedButton = Qt::NoButton;
		_dragging	   = false;

		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void Renderer::wheelEvent( QWheelEvent * const p_event ) { _inputManager.zoomBy( p_event->angleDelta().y() ); }

	QPoint Renderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatio();
		const int	px	= int( p_logicalPos.x() * dpr );
		const int	py	= int( p_logicalPos.y() * dpr );

		return { px, py };
	}

	void Renderer::_onKBLayoutChange( const Events::KeyboardLayoutChanged & p_e )
	{
		_layout = static_cast<KB_LAYOUT>( p_e.layout );
	}

} // namespace VTX::UI::QT::Window
