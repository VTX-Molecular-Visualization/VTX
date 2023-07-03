#ifndef __VTX_APP_OLD_CORE_EVENT_EVENT_MANAGER__
#define __VTX_APP_OLD_CORE_EVENT_EVENT_MANAGER__

#include "app/old/application/generic/base_lockable.hpp"
#include "app/old/application/generic/base_updatable.hpp"
#include "app/old/core/event/vtx_event.hpp"
#include "app/old/id.hpp"
#include <map>
#include <queue>
#include <set>

// #define DELAY_EVENTS

// TODO make this works with abstract event instead of VTXEvent. VTXEvent will inherits from abstract event.
namespace VTX::App::Old
{
	namespace Core::Event
	{
		class BaseEventReceiverVTX;
	} // namespace Core::Event

	namespace Manager
	{
		class EventManager final : public Application::Generic::BaseUpdatable, public Application::Generic::BaseLockable
		{
		  public:
			inline static EventManager & get()
			{
				static EventManager instance;
				return instance;
			}

			void registerEventReceiverVTX( const VTX::App::Old::Core::Event::VTX_EVENT &,
										   VTX::App::Old::Core::Event::BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const VTX::App::Old::Core::Event::VTX_EVENT &,
											 VTX::App::Old::Core::Event::BaseEventReceiverVTX * const );

			virtual void update( const float & p_deltaTime ) override;

			void fireEventVTX( VTX::App::Old::Core::Event::VTXEvent * const );

			void freezeEvent( const bool p_freeze );

		  private:
			std::map<VTX::App::Old::Core::Event::VTX_EVENT, std::set<VTX::App::Old::Core::Event::BaseEventReceiverVTX *>>
				_receiversVTX
				= std::map<VTX::App::Old::Core::Event::VTX_EVENT,
						   std::set<VTX::App::Old::Core::Event::BaseEventReceiverVTX *>>(); // VTX receivers mapped on
																					   // event ID.

			// Event queues.
			std::queue<VTX::App::Old::Core::Event::VTXEvent *> _eventQueueVTX
				= std::queue<VTX::App::Old::Core::Event::VTXEvent *>();

			bool _freeze = false;

			EventManager()									 = default;
			EventManager( const EventManager & )			 = delete;
			EventManager & operator=( const EventManager & ) = delete;
			~EventManager();

			void _flushVTXEvent( VTX::App::Old::Core::Event::VTXEvent * const );
		};
	} // namespace Manager
} // namespace VTX::App::Old
#endif
