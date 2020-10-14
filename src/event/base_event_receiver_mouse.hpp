#ifndef __VTX_BASE_EVENT_RECEIVER_MOUSE__
#define __VTX_BASE_EVENT_RECEIVER_MOUSE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include <QMouseEvent>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverMouse : public BaseEventReceiver<QMouseEvent>
		{
		  public:
			BaseEventReceiverMouse();
			virtual ~BaseEventReceiverMouse();
		};
	} // namespace Event
} // namespace VTX
#endif
