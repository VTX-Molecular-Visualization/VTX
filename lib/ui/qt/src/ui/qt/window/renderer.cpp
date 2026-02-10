#include "ui/qt/window/renderer.hpp"
#include "ui/qt/helper.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Window
{

	Renderer::Renderer() : QWindow(), _inputManager( App::INPUT() ) {}

	void Renderer::keyPressEvent( QKeyEvent * const p_event )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		switch ( p_event->key() )
		{
		case Qt::Key::Key_W: _inputManager.setMoveFront( true ); break;
		case Qt::Key::Key_S: _inputManager.setMoveBack( true ); break;
		case Qt::Key::Key_A: _inputManager.setMoveLeft( true ); break;
		case Qt::Key::Key_D: _inputManager.setMoveRight( true ); break;
		case Qt::Key::Key_R: _inputManager.setMoveUp( true ); break;
		case Qt::Key::Key_F: _inputManager.setMoveDown( true ); break;
		case Qt::Key::Key_Q: _inputManager.setRotateLeft( true ); break;
		case Qt::Key::Key_E: _inputManager.setRotateRight( true ); break;
		default: break;
		}

		_handleModifiers();
	}

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event )
	{
		if ( p_event->isAutoRepeat() )
		{
			return;
		}

		switch ( p_event->key() )
		{
		case Qt::Key::Key_W: _inputManager.setMoveFront( false ); break;
		case Qt::Key::Key_S: _inputManager.setMoveBack( false ); break;
		case Qt::Key::Key_A: _inputManager.setMoveLeft( false ); break;
		case Qt::Key::Key_D: _inputManager.setMoveRight( false ); break;
		case Qt::Key::Key_R: _inputManager.setMoveUp( false ); break;
		case Qt::Key::Key_F: _inputManager.setMoveDown( false ); break;
		case Qt::Key::Key_Q: _inputManager.setRotateLeft( false ); break;
		case Qt::Key::Key_E: _inputManager.setRotateRight( false ); break;
		default: break;
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

} // namespace VTX::UI::QT::Window
