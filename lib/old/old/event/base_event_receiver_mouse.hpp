#ifndef __VTX_BASE_EVENT_RECEIVER_MOUSE__
#define __VTX_BASE_EVENT_RECEIVER_MOUSE__

#include "base_event_receiver_input.hpp"
#include <QMouseEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverMouse : public BaseEventReceiverInput<QMouseEvent>
		{
		  public:
			BaseEventReceiverMouse();
			virtual ~BaseEventReceiverMouse();
		};
	} // namespace Event
} // namespace VTX
#endif
