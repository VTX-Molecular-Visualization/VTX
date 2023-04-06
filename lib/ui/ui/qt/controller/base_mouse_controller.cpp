#include "base_mouse_controller.hpp"
// #include "qt/application_qt.hpp"
// #include "qt/main_window.hpp"
// #include "qt/widget/render/opengl_widget.hpp"
#include <QCursor>
#include <QPoint>
#include <QRect>
#include <QWidget>
#include <util/math.hpp>

namespace VTX::UI::QT::Controller
{
	void BaseMouseController::receiveEvent( const QMouseEvent & p_event )
	{
		if ( isActive() == false )
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

	void BaseMouseController::receiveEvent( const QWheelEvent & p_event )
	{
		if ( isActive() == false )
			return;

		switch ( p_event.type() )
		{
		case QEvent::Wheel: _handleMouseWheelEvent( p_event ); break;
		default: break;
		}
	}

	void BaseMouseController::update( const float & p_deltaTime ) {}

	void BaseMouseController::_handleMouseButtonDownEvent( const QMouseEvent & p_event )
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

	void BaseMouseController::_handleMouseButtonUpEvent( const QMouseEvent & p_event )
	{
		switch ( p_event.button() )
		{
		case Qt::MouseButton::LeftButton:
			_mouseLeftPressed = false;
			if ( _isLeftClickCanceled == false )
			{
				_onMouseLeftClick( p_event.pos().x(), p_event.pos().y() );
			}
			break;
		case Qt::MouseButton::RightButton:
			_mouseRightPressed = false;
			if ( _isRightClickCanceled == false )
			{
				_onMouseRightClick( p_event.pos().x(), p_event.pos().y() );
			}
			break;
		case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = false; break;
		default: break;
		}
	}

	void BaseMouseController::_handleMouseDoubleClickEvent( const QMouseEvent & p_event )
	{
		switch ( p_event.button() )
		{
		case Qt::MouseButton::LeftButton:
			_onMouseLeftDoubleClick( p_event.pos().x(), p_event.pos().y() );
			_isLeftClickCanceled = true;
			break;
		case Qt::MouseButton::RightButton: break;
		case Qt::MouseButton::MiddleButton: break;
		default: break;
		}
	}

	void BaseMouseController::_handleMouseMotionEvent( const QMouseEvent & p_event )
	{
		Vec2i mousePosition = Vec2i();
		mousePosition.x		= p_event.pos().x();
		mousePosition.y		= p_event.pos().y();

		_deltaMousePosition += ( mousePosition - _mousePosition );
		_mousePosition = mousePosition;

		if ( _mouseLeftPressed )
		{
			if ( Util::Math::distance( (Vec2f)mousePosition, (Vec2f)_mouseLeftClickPosition ) > CLICK_MAX_DISTANCE )
			{
				_isLeftClickCanceled = true;
			}
		}

		if ( _mouseRightPressed )
		{
			if ( Util::Math::distance( (Vec2f)mousePosition, (Vec2f)_mouseRightClickPosition ) > CLICK_MAX_DISTANCE )
			{
				_isRightClickCanceled = true;
			}
		}
	}

	void BaseMouseController::_handleMouseWheelEvent( const QWheelEvent & p_event )
	{
		if ( p_event.modifiers().testFlag( Qt::AltModifier ) )
			_deltaMouseWheel = p_event.angleDelta().x();
		else
			_deltaMouseWheel = p_event.angleDelta().y();
	}

	/*
	bool BaseMouseController::_mouseHoveringRenderWidget() const
	{
		const QWidget & renderWidget  = VTXApp::get().getMainWindow().getOpenGLWidget();
		const QRect &	renderRect	  = renderWidget.contentsRect();
		const QPoint	localMousePos = renderWidget.mapFromGlobal( QCursor::pos() );

		return renderRect.contains( localMousePos );
	}
	*/

} // namespace VTX::UI::QT::Controller
