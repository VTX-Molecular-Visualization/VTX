#ifndef __VTX_BASE_EVENT_RECEIVER_WHEEL__
#define __VTX_BASE_EVENT_RECEIVER_WHEEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include <QWheelEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverWheel : public BaseEventReceiver<QWheelEvent>
		{
		  public:
			BaseEventReceiverWheel();
			virtual ~BaseEventReceiverWheel();
		};
	} // namespace Event
} // namespace VTX
#endif
