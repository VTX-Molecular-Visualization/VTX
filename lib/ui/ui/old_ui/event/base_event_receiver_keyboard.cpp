#include "base_event_receiver_keyboard.hpp"
#include "event_manager.hpp"
#include <util/logger.hpp>

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverKeyboard::BaseEventReceiverKeyboard()
		{
			UI::Event::EventManager::get().registerEventReceiverKeyboard( this );
		}
		BaseEventReceiverKeyboard::~BaseEventReceiverKeyboard()
		{
			UI::Event::EventManager::get().unregisterEventReceiverKeyboard( this );
		}
	} // namespace Event
} // namespace VTX
