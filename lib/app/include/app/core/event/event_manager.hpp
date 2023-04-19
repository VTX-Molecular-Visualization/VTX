#ifndef __VTX_APP_CORE_EVENT_EVENT_MANAGER__
#define __VTX_APP_CORE_EVENT_EVENT_MANAGER__

#include "app/event/vtx_event.hpp"
#include "app/old_app/generic/base_lockable.hpp"
#include "app/old_app/generic/base_updatable.hpp"
#include "app/old_app/id.hpp"
#include <map>
#include <queue>
#include <set>

// #define DELAY_EVENTS

// TODO make this works with abstract event instead of VTXEvent. VTXEvent will inherits from abstract event.
namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverVTX;
	} // namespace Event

	namespace Core::Event
	{
		class EventManager final : public Generic::BaseUpdatable, public Generic::BaseLockable
		{
		  public:
			inline static EventManager & get()
			{
				static EventManager instance;
				return instance;
			}

			void registerEventReceiverVTX( const VTX::Event::VTX_EVENT &, VTX::Event::BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const VTX::Event::VTX_EVENT &, VTX::Event::BaseEventReceiverVTX * const );

			virtual void update( const float & p_deltaTime ) override;

			void fireEventVTX( VTX::Event::VTXEvent * const );

			void freezeEvent( const bool p_freeze );

		  private:
			std::map<VTX::Event::VTX_EVENT, std::set<VTX::Event::BaseEventReceiverVTX *>> _receiversVTX
				= std::map<VTX::Event::VTX_EVENT,
						   std::set<VTX::Event::BaseEventReceiverVTX *>>(); // VTX receivers mapped on event ID.

			// Event queues.
			std::queue<VTX::Event::VTXEvent *> _eventQueueVTX = std::queue<VTX::Event::VTXEvent *>();

			bool _freeze = false;

			EventManager()									 = default;
			EventManager( const EventManager & )			 = delete;
			EventManager & operator=( const EventManager & ) = delete;
			~EventManager();

			void _flushVTXEvent( VTX::Event::VTXEvent * const );
		};
	} // namespace Core::Event

	// TODO when event manager will be abstracted, this call must be moved in event/vtx_event.hpp
	inline void VTX_EVENT( VTX::Event::VTXEvent * const p_event )
	{
		VTX::Core::Event::EventManager::get().fireEventVTX( p_event );
	}
} // namespace VTX
#endif
