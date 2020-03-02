#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver_sdl.hpp"
#include "base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "generic/base_updatable.hpp"
#include <map>
#include <queue>
#include <set>

namespace VTX
{
	namespace Event
	{
		class EventManager : public Generic::BaseUpdatable
		{
		  public:
			using SetBaseEventReceiverSDLPtr			 = std::set<BaseEventReceiverSDL *>;
			using SetBaseEventReceiverVTXPtr			 = std::set<BaseEventReceiverVTX *>;
			using MapStringVectorBaseEventReceiverVTXPtr = std::map<Event::VTX_EVENT, SetBaseEventReceiverVTXPtr>;
			using QueueVTXEventPtr						 = std::queue<VTXEvent *>;

			void registerEventReceiverSDL( BaseEventReceiverSDL * const );
			void unregisterEventReceiverSDL( BaseEventReceiverSDL * const );
			void registerEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );

			void fireEvent( VTXEvent * const );

			virtual void update( const double p_deltaTime ) override;

		  private:
			SetBaseEventReceiverSDLPtr			   _receiversSDL = SetBaseEventReceiverSDLPtr();
			MapStringVectorBaseEventReceiverVTXPtr _receiversVTX = MapStringVectorBaseEventReceiverVTXPtr();
			QueueVTXEventPtr					   _eventQueue	 = QueueVTXEventPtr();

			void _flushEvent( VTXEvent * );
		};
	} // namespace Event
} // namespace VTX
#endif
