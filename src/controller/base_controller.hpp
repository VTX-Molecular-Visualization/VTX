#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_sdl.hpp"
#include "generic/base_collectionable.hpp"
#include "generic/base_updatable.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseController :
			public Generic::BaseUpdatable,
			public Generic::BaseCollectionable,
			public Event::BaseEventReceiverSDL
		{
		  public:
			virtual void init() override { Event::BaseEventReceiverSDL::registerEvents(); }
			virtual void clean() override { Event::BaseEventReceiverSDL::unregisterEvents(); }
		};
	} // namespace Controller
} // namespace VTX
#endif
