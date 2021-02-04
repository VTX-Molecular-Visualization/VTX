#ifndef __VTX_BASE_MOUSE_CONTROLLER__
#define __VTX_BASE_MOUSE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include "define.hpp"
#include "event/base_event_receiver_mouse.hpp"
#include "event/base_event_receiver_wheel.hpp"
#include <iostream>
#include <set>

namespace VTX
{
	namespace Controller
	{
		class BaseMouseController :
			virtual public BaseController,
			public Event::BaseEventReceiverMouse,
			public Event::BaseEventReceiverWheel
		{
		  public:
			virtual void receiveEvent( const QMouseEvent & p_event ) override
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

			virtual void receiveEvent( const QWheelEvent & p_event ) override
			{
				if ( isActive() == false )
					return;

				switch ( p_event.type() )
				{
				case QEvent::Wheel: _handleMouseWheelEvent( p_event ); break;
				default: break;
				}
			}

		  protected:
			bool  _mouseLeftPressed	  = false;
			bool  _mouseRightPressed  = false;
			bool  _mouseMiddlePressed = false;
			Vec2i _mousePosition	  = Vec2i();
			Vec2i _deltaMousePosition = Vec2i();
			int	  _deltaMouseWheel	  = 0;

			virtual void _handleMouseButtonDownEvent( const QMouseEvent & p_event )
			{
				switch ( p_event.button() )
				{
				case Qt::MouseButton::LeftButton: _mouseLeftPressed = true; break;
				case Qt::MouseButton::RightButton: _mouseRightPressed = true; break;
				case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = true; break;
				default: break;
				}

				// Save current position.
				_mousePosition.x = p_event.pos().x();
				_mousePosition.y = p_event.pos().y();
			}

			virtual void _handleMouseButtonUpEvent( const QMouseEvent & p_event )
			{
				switch ( p_event.button() )
				{
				case Qt::MouseButton::LeftButton: _mouseLeftPressed = false; break;
				case Qt::MouseButton::RightButton: _mouseRightPressed = false; break;
				case Qt::MouseButton::MiddleButton: _mouseMiddlePressed = false; break;
				default: break;
				}
			}

			virtual void _handleMouseMotionEvent( const QMouseEvent & p_event )
			{
				Vec2i mousePosition = Vec2i();
				mousePosition.x		= p_event.pos().x();
				mousePosition.y		= p_event.pos().y();

				_deltaMousePosition += ( mousePosition - _mousePosition );
				_mousePosition = mousePosition;
			}

			virtual void _handleMouseWheelEvent( const QWheelEvent & p_event )
			{
				_deltaMouseWheel += p_event.angleDelta().y();
			}
		};
	} // namespace Controller
} // namespace VTX
#endif
