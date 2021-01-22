#include "base_event_receiver_wheel.hpp"
#include "event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverWheel ::BaseEventReceiverWheel() { EventManager::get().registerEventReceiverWheel( this ); }

		BaseEventReceiverWheel ::~BaseEventReceiverWheel() { EventManager::get().unregisterEventReceiverWheel( this ); }

	} // namespace Event
} // namespace VTX
