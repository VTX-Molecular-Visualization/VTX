#ifndef __VTX_BASE_MOUSE_CONTROLLER__
#define __VTX_BASE_MOUSE_CONTROLLER__

#include "base_controller.hpp"
#include "define.hpp"
#include "event/base_event_receiver_mouse.hpp"
#include "event/base_event_receiver_wheel.hpp"

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
			virtual void receiveEvent( const QMouseEvent & p_event ) override;
			virtual void receiveEvent( const QWheelEvent & p_event ) override;
			virtual void update( const float & p_deltaTime ) override;

		  protected:
			bool _mouseLeftPressed	 = false;
			bool _mouseRightPressed	 = false;
			bool _mouseMiddlePressed = false;
			bool _mouseLeftClick	 = false;
			bool _mouseRightClick	 = false;

			Vec2i _mousePosition	  = Vec2i();
			Vec2i _deltaMousePosition = Vec2i();
			int	  _deltaMouseWheel	  = 0;

			virtual void _handleMouseButtonDownEvent( const QMouseEvent & p_event );
			virtual void _handleMouseButtonUpEvent( const QMouseEvent & p_event );
			virtual void _handleMouseMotionEvent( const QMouseEvent & p_event );
			virtual void _handleMouseWheelEvent( const QWheelEvent & p_event );

			bool _mouseHoveringRenderWidget() const;

		  private:
			Vec2i _mouseLeftClickPosition  = Vec2i();
			Vec2i _mouseRightClickPosition = Vec2i();
			float _leftClickTimeStamp	   = -1.0f;
			float _rightClickTimeStamp	   = -1.0f;

			void _updateLeftClick( const float & p_deltaTime );
			void _updateRightClick( const float & p_deltaTime );
		};
	} // namespace Controller
} // namespace VTX
#endif
