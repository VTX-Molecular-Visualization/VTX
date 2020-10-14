#ifndef __VTX_BASE_EVENT_RECEIVER_KEYBOARD__
#define __VTX_BASE_EVENT_RECEIVER_KEYBOARD__

#ifdef _MSC_VER
#pragma once
#endif

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
		};
	} // namespace Event
} // namespace VTX
#endif
