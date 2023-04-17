#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#include "base_event_receiver_vtx.hpp"
#include "app/old_app/event/event.hpp"
#include "app/old_app/generic/base_lockable.hpp"
#include "app/old_app/generic/base_updatable.hpp"
#include "app/old_app/id.hpp"
#include <map>
#include <queue>
#include <set>

// #define DELAY_EVENTS

namespace VTX
{
	namespace Event
	{
		class EventManager final : public Generic::BaseUpdatable, public Generic::BaseLockable
		{
		  public:
			inline static EventManager & get()
			{
				static EventManager instance;
				return instance;
			}

			void registerEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );

			virtual void update( const float & p_deltaTime ) override;

			void fireEventVTX( VTXEvent * const );

			void freezeEvent( const bool p_freeze );

		  private:
			// Input events.
			std::map<Event::VTX_EVENT, std::set<BaseEventReceiverVTX *>> _receiversVTX
				= std::map<Event::VTX_EVENT, std::set<BaseEventReceiverVTX *>>(); // VTX receivers mapped on event ID.

			// Event queues.
			std::queue<VTXEvent *> _eventQueueVTX = std::queue<VTXEvent *>();

			bool _freeze = false;

			EventManager()									 = default;
			EventManager( const EventManager & )			 = delete;
			EventManager & operator=( const EventManager & ) = delete;
			~EventManager();

			void _flushVTXEvent( VTXEvent * const );
		};
	} // namespace Event

	inline void VTX_EVENT( VTX::Event::VTXEvent * const p_event )
	{
		Event::EventManager::get().fireEventVTX( p_event );
	}
} // namespace VTX
#endif
