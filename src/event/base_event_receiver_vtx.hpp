#ifndef __VTX_BASE_EVENT_RECEIVER_VTX__
#define __VTX_BASE_EVENT_RECEIVER_VTX__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include "define.hpp"
#include "event.hpp"
#include <string>
#include <vector>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverVTX : public BaseEventReceiver<VTXEvent>
		{
		  protected:
			virtual void				   _registerEvents() override final;
			virtual void				   _unregisterEvents() override final;
			virtual std::vector<VTX_EVENT> _getEvents() const { return std::vector<VTX_EVENT>(); }
		};
	} // namespace Event
} // namespace VTX
#endif
