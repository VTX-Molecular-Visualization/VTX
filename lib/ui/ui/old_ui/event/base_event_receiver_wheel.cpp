#include "base_event_receiver_wheel.hpp"
#include "event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverWheel ::BaseEventReceiverWheel()
		{
			UI::Event::EventManager::get().registerEventReceiverWheel( this );
		}

		BaseEventReceiverWheel ::~BaseEventReceiverWheel()
		{
			UI::Event::EventManager::get().unregisterEventReceiverWheel( this );
		}

	} // namespace Event
} // namespace VTX
