#include "ui/old_ui/event/base_event_receiver_mouse.hpp"
#include "ui/old_ui/event/event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverMouse::BaseEventReceiverMouse()
		{
			UI::Event::EventManager::get().registerEventReceiverMouse( this );
		}

		BaseEventReceiverMouse::~BaseEventReceiverMouse()
		{
			UI::Event::EventManager::get().unregisterEventReceiverMouse( this );
		}

	} // namespace Event
} // namespace VTX
