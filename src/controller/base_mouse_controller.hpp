#ifndef __VTX_BASE_MOUSE_CONTROLLER__
#define __VTX_BASE_MOUSE_CONTROLLER__

#include "base_controller.hpp"
#include "define.hpp"
#include "event/base_event_receiver_focus.hpp"
#include "event/base_event_receiver_mouse.hpp"
#include "event/base_event_receiver_wheel.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseMouseController :
			virtual public BaseController,
			public Event::BaseEventReceiverMouse,
			public Event::BaseEventReceiverWheel,
			public Event::BaseEventReceiverFocus
		{
		  public:
			inline static const int CLICK_MAX_DISTANCE = 3;

		  public:
			virtual void receiveEvent( const QMouseEvent & p_event ) override;
			virtual void receiveEvent( const QWheelEvent & p_event ) override;
			virtual void receiveEvent( const QFocusEvent & p_event ) override;
			virtual void update( const float & p_deltaTime ) override;

		  protected:
			bool _mouseLeftPressed	 = false;
			bool _mouseRightPressed	 = false;
			bool _mouseMiddlePressed = false;

			Vec2i _mousePosition	  = Vec2i();
			Vec2i _deltaMousePosition = Vec2i();
			int	  _deltaMouseWheel	  = 0;

			virtual void _handleMouseButtonDownEvent( const QMouseEvent & p_event );
			virtual void _handleMouseButtonUpEvent( const QMouseEvent & p_event );
			virtual void _handleMouseDoubleClickEvent( const QMouseEvent & p_event );
			virtual void _handleMouseMotionEvent( const QMouseEvent & p_event );
			virtual void _handleMouseWheelEvent( const QWheelEvent & p_event );

			virtual void _onMouseLeftClick( const uint p_x, const uint p_y ) {}
			virtual void _onMouseRightClick( const uint p_x, const uint p_y ) {}

			virtual void _onMouseLeftDoubleClick( const uint p_x, const uint p_y ) {}

			bool _isLeftClickCanceled  = false;
			bool _isRightClickCanceled = false;

			// bool _mouseHoveringRenderWidget() const;

		  private:
			void  _resetState();
			Vec2i _mouseLeftClickPosition  = Vec2i();
			Vec2i _mouseRightClickPosition = Vec2i();
		};
	} // namespace Controller
} // namespace VTX
#endif
