#ifndef __VTX_BASE_EVENT_RECEIVER_KEYBOARD__
#define __VTX_BASE_EVENT_RECEIVER_KEYBOARD__

#include "base_event_receiver_input.hpp"
#include <QKeyEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverKeyboard : public BaseEventReceiverInput<QKeyEvent>
		{
		  public:
			BaseEventReceiverKeyboard();
			virtual ~BaseEventReceiverKeyboard();
		};
	} // namespace Event
} // namespace VTX
#endif
