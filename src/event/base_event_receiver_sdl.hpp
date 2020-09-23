#ifndef __VTX_BASE_EVENT_RECEIVER_SDL__
#define __VTX_BASE_EVENT_RECEIVER_SDL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include "id.hpp"
#include <SDL.h>
#include <vector>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverSDL : public BaseEventReceiver<SDL_Event>
		{
		  protected:
			void							_registerEvents() override final;
			void							_unregisterEvents() override final;
			virtual std::vector<ID::VTX_ID> _getUIItems() const { return std::vector<ID::VTX_ID>(); }
		};
	} // namespace Event
} // namespace VTX
#endif
