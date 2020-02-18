#ifndef __VTX_BASE_EVENT_RECEIVER_VTX__
#define __VTX_BASE_EVENT_RECEIVER_VTX__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include <string>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverVTX : public BaseEventReceiver<std::string>
		{
		  public:
			virtual void registerEvents() override;
			virtual void unregisterEvents() override;
		};
	} // namespace Event
} // namespace VTX
#endif
