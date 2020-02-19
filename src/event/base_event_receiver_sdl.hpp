#ifndef __VTX_BASE_EVENT_RECEIVER_SDL__
#define __VTX_BASE_EVENT_RECEIVER_SDL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
#include <SDL2/SDL.h>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverSDL : public BaseEventReceiver<SDL_Event>
		{
		  protected:
			virtual void _registerEvents() override;
			virtual void _unregisterEvents() override;
		};
	} // namespace Event
} // namespace VTX
#endif
