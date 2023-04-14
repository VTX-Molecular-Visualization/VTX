#ifndef __VTX_BASE_EVENT_RECEIVER__
#define __VTX_BASE_EVENT_RECEIVER__

#include "event.hpp"

namespace VTX
{
	namespace Event
	{
		class EventManager;

		template<typename T>
		class BaseEventReceiver
		{
			friend EventManager;

		  protected:
			virtual void receiveEvent( const T & p_event ) {}
		};
	} // namespace Event
} // namespace VTX
#endif
