#include "base_mouse_controller.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/render/opengl_widget.hpp"
#include "vtx_app.hpp"
#include <QCursor>
#include <QPoint>
#include <QRect>
#include <QWidget>

namespace VTX
{
	namespace Controller
	{
		void BaseMouseController::receiveEvent( const QMouseEvent & p_event )
		{
			if ( isActive() == false )
				return;

			switch ( p_event.type() )
			{
			case QEvent::MouseButtonPress: _handleMouseButtonDownEvent( p_event ); break;
			case QEvent::MouseButtonRelease: _handleMouseButtonUpEvent( p_event ); break;
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

		void BaseMouseController::update( const float & p_deltaTime )
		{
			_updateLeftClick( p_deltaTime );
			_updateRightClick( p_deltaTime );
		}

		void BaseMouseController::_handleMouseButtonDownEvent( const QMouseEvent & p_event )
		{
			// Save current position.
			_mousePosition.x = p_event.pos().x();
			_mousePosition.y = p_event.pos().y();

			switch ( p_event.button() )
			{
			case Qt::MouseButton::LeftButton:
				_mouseLeftPressed		= true;
				_leftClickTimeStamp		= CLICK_TIME;
				_mouseLeftClickPosition = _mousePosition;
				break;
			case Qt::MouseButton::RightButton:
				_mouseRightPressed		 = true;
				_rightClickTimeStamp	 = CLICK_TIME;
				_mouseRightClickPosition = _mousePosition;
				break;
			case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = true; break;
			default: break;
			}
		}

		void BaseMouseController::_handleMouseButtonUpEvent( const QMouseEvent & p_event )
		{
			switch ( p_event.button() )
			{
			case Qt::MouseButton::LeftButton: _mouseLeftPressed = false; break;
			case Qt::MouseButton::RightButton: _mouseRightPressed = false; break;
			case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = false; break;
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
		}

		void BaseMouseController::_handleMouseWheelEvent( const QWheelEvent & p_event )
		{
			_deltaMouseWheel = p_event.angleDelta().y();
		}

		bool BaseMouseController::_mouseHoveringRenderWidget() const
		{
			const QWidget & renderWidget  = VTXApp::get().getMainWindow().getOpenGLWidget();
			const QRect &	renderRect	  = renderWidget.contentsRect();
			const QPoint	localMousePos = renderWidget.mapFromGlobal( QCursor::pos() );

			return renderRect.contains( localMousePos );
		}

		void BaseMouseController::_updateLeftClick( const float & p_deltaTime )
		{
			if ( _leftClickTimeStamp > 0 )
			{
				const Vec2i mouseMoveOffset = _mousePosition - _mouseLeftClickPosition;
				const int	mouseSquareDist
					= mouseMoveOffset.x * mouseMoveOffset.x + mouseMoveOffset.y * mouseMoveOffset.y;

				if ( mouseSquareDist > CLICK_SQR_DISTANCE )
				{
					_mouseLeftClick		= false;
					_leftClickTimeStamp = 0;
				}
				else if ( !_mouseLeftPressed )
				{
					_mouseLeftClick		= true;
					_leftClickTimeStamp = 0;
				}
				else
				{
					_mouseLeftClick = false;
				}
			}
			else
			{
				_mouseLeftClick = false;
			}
			_leftClickTimeStamp -= p_deltaTime;
		}
		void BaseMouseController::_updateRightClick( const float & p_deltaTime )
		{
			if ( _rightClickTimeStamp > 0 )
			{
				const Vec2i mouseMoveOffset = _mousePosition - _mouseRightClickPosition;
				const int	mouseSquareDist
					= mouseMoveOffset.x * mouseMoveOffset.x + mouseMoveOffset.y * mouseMoveOffset.y;

				if ( mouseSquareDist > CLICK_SQR_DISTANCE )
				{
					_mouseRightClick	 = false;
					_rightClickTimeStamp = 0;
				}
				else if ( !_mouseRightPressed )
				{
					_mouseRightClick	 = true;
					_rightClickTimeStamp = 0;
				}
				else
				{
					_mouseRightClick = false;
				}
			}
			else
			{
				_mouseRightClick = false;
			}

			_rightClickTimeStamp -= p_deltaTime;
		}

	} // namespace Controller
} // namespace VTX
