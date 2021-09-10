#ifndef __VTX_BASE_EVENT_RECEIVER_KEYBOARD__
#define __VTX_BASE_EVENT_RECEIVER_KEYBOARD__

#include "base_event_receiver.hpp"
#include <QKeyEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverKeyboard : public BaseEventReceiver<QKeyEvent>
		{
		  public:
			BaseEventReceiverKeyboard();
			virtual ~BaseEventReceiverKeyboard();

			virtual void clear();
		};
	} // namespace Event
} // namespace VTX
#endif
