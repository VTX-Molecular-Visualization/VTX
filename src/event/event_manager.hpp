#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver_sdl.hpp"
#include "base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include <map>
#include <queue>
#include <set>

#define DELAY_EVENTS

namespace VTX
{
	namespace Event
	{
		class EventManager : public Generic::BaseUpdatable
		{
		  public:
			using SetBaseEventReceiverSDLPtr			 = std::set<BaseEventReceiverSDL *>;
			using MapStringVectorBaseEventReceiverSDLPtr = std::map<ID::VTX_ID, SetBaseEventReceiverSDLPtr>;

			using SetBaseEventReceiverVTXPtr			 = std::set<BaseEventReceiverVTX *>;
			using MapStringVectorBaseEventReceiverVTXPtr = std::map<Event::VTX_EVENT, SetBaseEventReceiverVTXPtr>;

			using QueueVTXEventPtr = std::queue<VTXEvent *>;

			void registerEventReceiverSDL( const ID::VTX_ID &, BaseEventReceiverSDL * const );
			void unregisterEventReceiverSDL( const ID::VTX_ID &, BaseEventReceiverSDL * const );
			void registerEventReceiverSDL( BaseEventReceiverSDL * const );
			void unregisterEventReceiverSDL( BaseEventReceiverSDL * const );
			void registerEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );

			void fireEvent( VTXEvent * const );

			virtual void update( const double p_deltaTime ) override;

		  private:
			// SDL receivers mapped on window ID.
			MapStringVectorBaseEventReceiverSDLPtr _receiversSDL = MapStringVectorBaseEventReceiverSDLPtr();
			// SDL receivers receiving all events.
			SetBaseEventReceiverSDLPtr _receiversSDLGlobal = SetBaseEventReceiverSDLPtr();
			// VTX receivers mapped on event ID.
			MapStringVectorBaseEventReceiverVTXPtr _receiversVTX = MapStringVectorBaseEventReceiverVTXPtr();
			// Current VTX event queue.
			QueueVTXEventPtr _eventQueue = QueueVTXEventPtr();

			void _handlerWindowEvent( const SDL_WindowEvent & );
			void _flushVTXEvent( VTXEvent * const );
		};
	} // namespace Event

} // namespace VTX
#endif
