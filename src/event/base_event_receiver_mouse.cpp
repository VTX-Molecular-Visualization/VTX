#include "base_event_receiver_mouse.hpp"
#include "event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverMouse ::BaseEventReceiverMouse() { EventManager::get().registerEventReceiverMouse( this ); }

		BaseEventReceiverMouse ::~BaseEventReceiverMouse() { EventManager::get().unregisterEventReceiverMouse( this ); }

	} // namespace Event
} // namespace VTX
