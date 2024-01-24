// #ifndef __VTX_BASE_EVENT_FIRERER_INPUT__
// #define __VTX_BASE_EVENT_FIRERER_INPUT__
//
// #include "input_manager.hpp"
// #include <app/old/id.hpp>
//
// namespace VTX::UI::Qt::Controller
//{
//	class BaseEventFirererInput
//	{
//	  public:
//		virtual const App::Old::VTX_ID getEventFirererId() const = 0;
//
//	  protected:
//		void _fireEventInput( QKeyEvent * const p_event )
//		{
//			InputManager::get().fireEventKeyboard( p_event, getEventFirererId() );
//			p_event->accept();
//		}
//
//		void _fireEventInput( QMouseEvent * const p_event )
//		{
//			InputManager::get().fireEventMouse( p_event, getEventFirererId() );
//			p_event->accept();
//		}
//
//		void _fireEventInput( QWheelEvent * const p_event )
//		{
//			InputManager::get().fireEventWheel( p_event, getEventFirererId() );
//			p_event->accept();
//		}
//	};
// } // namespace VTX::UI::Qt::Controller
// #endif
