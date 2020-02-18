#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver_sdl.hpp"
#include "generic/base_updatable.hpp"
#include <set>

namespace VTX
{
	namespace Event
	{
		class EventManager : public Generic::BaseUpdatable
		{
		  public:
			using SetBaseEventReceiverSDLPtr = std::set<BaseEventReceiverSDL *>;

			virtual void update( const double p_deltaTime ) override;
			void		 registerEventReceiverSDL( BaseEventReceiverSDL * const );
			void		 unregisterEventReceiverSDL( BaseEventReceiverSDL * const );

		  private:
			SetBaseEventReceiverSDLPtr _receiversSDL = SetBaseEventReceiverSDLPtr();
		};
	} // namespace Event
} // namespace VTX
#endif
