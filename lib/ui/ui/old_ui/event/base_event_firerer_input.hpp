#ifndef __VTX_BASE_EVENT_FIRERER_INPUT__
#define __VTX_BASE_EVENT_FIRERER_INPUT__

#include "event_manager.hpp"
#include "id.hpp"

namespace VTX::UI::Event
{
	class BaseEventFirererInput
	{
	  public:
		virtual const ID::VTX_ID getEventFirererId() const = 0;

	  protected:
		void _fireEventInput( QKeyEvent * const p_event )
		{
			EventManager::get().fireEventKeyboard( p_event, getEventFirererId() );
			p_event->accept();
		}

		void _fireEventInput( QMouseEvent * const p_event )
		{
			EventManager::get().fireEventMouse( p_event, getEventFirererId() );
			p_event->accept();
		}

		void _fireEventInput( QWheelEvent * const p_event )
		{
			EventManager::get().fireEventWheel( p_event, getEventFirererId() );
			p_event->accept();
		}
	};
} // namespace VTX::UI::Event
#endif
