#ifndef __VTX_BASE_EVENT_RECEIVER__
#define __VTX_BASE_EVENT_RECEIVER__

#include "event.hpp"
#include <iostream>

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class BaseEventReceiver
		{
		  public:
			virtual void receiveEvent( const T & p_event ) {}
		};
	} // namespace Event
} // namespace VTX
#endif
