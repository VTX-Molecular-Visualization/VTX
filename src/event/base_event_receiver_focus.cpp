#include "base_event_receiver_focus.hpp"
#include "event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverFocus::BaseEventReceiverFocus() { EventManager::get().registerEventReceiverFocus( this ); }
		BaseEventReceiverFocus::~BaseEventReceiverFocus() { EventManager::get().unregisterEventReceiverFocus( this ); }

	} // namespace Event
} // namespace VTX
