#ifndef __VTX_BASE_EVENT_RECEIVER_FOCUS__
#define __VTX_BASE_EVENT_RECEIVER_FOCUS__

#include "base_event_receiver_input.hpp"
#include <QFocusEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverFocus : public BaseEventReceiverInput<QFocusEvent>
		{
		  public:
			BaseEventReceiverFocus();
			virtual ~BaseEventReceiverFocus();
		};
	} // namespace Event
} // namespace VTX
#endif
