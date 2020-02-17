#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver_sdl.hpp"
#include <set>

namespace VTX
{
	namespace Event
	{
		class EventManager
		{
		  public:
			using SetBaseEventReceiverSDLPtr = std::set<BaseEventReceiverSDL *>;

			void registerEventReceiverSDL( BaseEventReceiverSDL * const p_receiver )
			{
				_receiversSDL.emplace( p_receiver );
			}

		  private:
			SetBaseEventReceiverSDLPtr _receiversSDL = SetBaseEventReceiverSDLPtr();
		};
	} // namespace Event
} // namespace VTX
#endif
