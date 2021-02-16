#ifndef __VTX_BASE_MOUSE_CONTROLLER__
#define __VTX_BASE_MOUSE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include "define.hpp"
#include "event/base_event_receiver_mouse.hpp"
#include "event/base_event_receiver_wheel.hpp"
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
			inline static const int	  CLICK_SQR_DISTANCE = 25;
			inline static const float CLICK_TIME		 = 0.275f;

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

			virtual void update( const float & p_deltaTime ) override
			{
				_updateLeftClick( p_deltaTime );
				_updateRightClick( p_deltaTime );
			}

		  protected:
			bool _mouseLeftPressed	 = false;
			bool _mouseRightPressed	 = false;
			bool _mouseMiddlePressed = false;
			bool _mouseLeftClick	 = false;
			bool _mouseRightClick	 = false;

			Vec2i _mousePosition	  = Vec2i();
			Vec2i _deltaMousePosition = Vec2i();
			int	  _deltaMouseWheel	  = 0;

			virtual void _handleMouseButtonDownEvent( const QMouseEvent & p_event )
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
				_deltaMouseWheel = p_event.angleDelta().y();
			}

		  private:
			Vec2i _mouseLeftClickPosition  = Vec2i();
			Vec2i _mouseRightClickPosition = Vec2i();
			float _leftClickTimeStamp	   = -1.0f;
			float _rightClickTimeStamp	   = -1.0f;

			void _updateLeftClick( const float & p_deltaTime )
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
			void _updateRightClick( const float & p_deltaTime )
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
		};
	} // namespace Controller
} // namespace VTX
#endif
