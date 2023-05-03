#ifndef __VTX_APP_CORE_EVENT_EVENT_MANAGER__
#define __VTX_APP_CORE_EVENT_EVENT_MANAGER__

#include "app/core/event/vtx_event.hpp"
#include "app/old_app/generic/base_lockable.hpp"
#include "app/old_app/generic/base_updatable.hpp"
#include "app/id.hpp"
#include <map>
#include <queue>
#include <set>

// #define DELAY_EVENTS

// TODO make this works with abstract event instead of VTXEvent. VTXEvent will inherits from abstract event.
namespace VTX::App
{
	namespace Core::Event
	{
		class BaseEventReceiverVTX;
	} // namespace Core::Event

	namespace Manager
	{
		class EventManager final : public Generic::BaseUpdatable, public Generic::BaseLockable
		{
		  public:
			inline static EventManager & get()
			{
				static EventManager instance;
				return instance;
			}

			void registerEventReceiverVTX( const VTX::App::Core::Event::VTX_EVENT &,
										   VTX::App::Core::Event::BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const VTX::App::Core::Event::VTX_EVENT &,
											 VTX::App::Core::Event::BaseEventReceiverVTX * const );

			virtual void update( const float & p_deltaTime ) override;

			void fireEventVTX( VTX::App::Core::Event::VTXEvent * const );

			void freezeEvent( const bool p_freeze );

		  private:
			std::map<VTX::App::Core::Event::VTX_EVENT, std::set<VTX::App::Core::Event::BaseEventReceiverVTX *>>
				_receiversVTX
				= std::map<VTX::App::Core::Event::VTX_EVENT,
						   std::set<VTX::App::Core::Event::BaseEventReceiverVTX *>>(); // VTX receivers mapped on
																					   // event ID.

			// Event queues.
			std::queue<VTX::App::Core::Event::VTXEvent *> _eventQueueVTX
				= std::queue<VTX::App::Core::Event::VTXEvent *>();

			bool _freeze = false;

			EventManager()									 = default;
			EventManager( const EventManager & )			 = delete;
			EventManager & operator=( const EventManager & ) = delete;
			~EventManager();

			void _flushVTXEvent( VTX::App::Core::Event::VTXEvent * const );
		};
	} // namespace Manager
} // namespace VTX::App
#endif
