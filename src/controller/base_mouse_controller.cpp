#include "base_mouse_controller.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/render/opengl_widget.hpp"
#include "vtx_app.hpp"

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

		void BaseMouseController::receiveEvent( const QFocusEvent & p_event )
		{
			if ( p_event.gotFocus() )
			{
				_resetState();
			}
		}

		void BaseMouseController::update( const float & p_deltaTime ) {}

		void BaseMouseController::_handleMouseButtonDownEvent( const QMouseEvent & p_event )
		{
			// Save current position.
			_mouseGlobalPosition.x = p_event.globalPos().x();
			_mouseGlobalPosition.y = p_event.globalPos().y();

			switch ( p_event.button() )
			{
			case Qt::MouseButton::LeftButton:
				_mouseLeftPressed		= true;
				_isLeftClickCanceled	= false;
				_mouseLeftClickPosition = _mouseGlobalPosition;
				if ( _mouseRightPressed )
				{
					_isRightClickCanceled = true;
				}
				break;
			case Qt::MouseButton::RightButton:
				_mouseRightPressed		 = true;
				_isRightClickCanceled	 = false;
				_mouseRightClickPosition = _mouseGlobalPosition;
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
			const Vec2i globalMousePosition = Vec2i( p_event.globalPos().x(), p_event.globalPos().y() );

			_deltaMousePosition += ( globalMousePosition - _mouseGlobalPosition );
			_mouseGlobalPosition = globalMousePosition;

			if ( _mouseLeftPressed )
			{
				if ( Util::Math::distance( Vec2f( globalMousePosition ), Vec2f( _mouseLeftClickPosition ) )
					 > CLICK_MAX_DISTANCE )
				{
					_isLeftClickCanceled = true;
				}
			}

			if ( _mouseRightPressed )
			{
				if ( Util::Math::distance( Vec2f( globalMousePosition ), Vec2f( _mouseRightClickPosition ) )
					 > CLICK_MAX_DISTANCE )
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

		void BaseMouseController::_resetState()
		{
			_mouseLeftPressed	  = false;
			_mouseRightPressed	  = false;
			_isLeftClickCanceled  = false;
			_isRightClickCanceled = false;

			_mouseGlobalPosition = Vec2i( QCursor::pos().x(), QCursor::pos().y() );
			_deltaMousePosition	 = VEC2I_ZERO;
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

	} // namespace Controller
} // namespace VTX
