#include "base_event_receiver_keyboard.hpp"
#include "event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverKeyboard::BaseEventReceiverKeyboard()
		{
			EventManager::get().registerEventReceiverKeyboard( this );
		}
		BaseEventReceiverKeyboard::~BaseEventReceiverKeyboard()
		{
			EventManager::get().unregisterEventReceiverKeyboard( this );
		}

		void BaseEventReceiverKeyboard::clear() {};

	} // namespace Event
} // namespace VTX
