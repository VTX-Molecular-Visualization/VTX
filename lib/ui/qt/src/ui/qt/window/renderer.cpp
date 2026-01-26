#include "ui/qt/window/renderer.hpp"
#include "ui/qt/helper.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/input/input_manager.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Window
{

	void Renderer::keyPressEvent( QKeyEvent * const p_event )
	{
		App::INPUT().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
	}

	void Renderer::keyReleaseEvent( QKeyEvent * const p_event )
	{
		App::INPUT().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
	}

	void Renderer::mousePressEvent( QMouseEvent * p_event )
	{
		App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );

		_pressedButton = p_event->button();
		_pressPos	   = p_event->position();
		_dragging	   = false;
	}

	void Renderer::mouseMoveEvent( QMouseEvent * p_event )
	{
		App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );

		if ( _pressedButton == Qt::NoButton )
		{
			return;
		}

		const int threshold = QGuiApplication::styleHints()->startDragDistance();
		if ( not _dragging && ( p_event->position() - _pressPos ).manhattanLength() >= threshold )
		{
			_dragging = true;
			emit dragged( _pressedButton );
		}
	}

	void Renderer::mouseReleaseEvent( QMouseEvent * p_event )
	{
		App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );

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

	void Renderer::wheelEvent( QWheelEvent * const p_event )
	{
		App::INPUT().handleMouseWheelEvent( Helper::qWheelEventToWheelEvent( *p_event ) );
	}

	QPoint Renderer::_toDevicePixels( const QPointF & p_logicalPos ) const
	{
		const qreal dpr = devicePixelRatio();
		const int	px	= int( p_logicalPos.x() * dpr );
		const int	py	= int( p_logicalPos.y() * dpr );

		return { px, py };
	}

} // namespace VTX::UI::QT::Window
