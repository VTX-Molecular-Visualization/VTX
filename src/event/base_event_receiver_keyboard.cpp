#include "base_event_receiver_keyboard.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverKeyboard ::BaseEventReceiverKeyboard() { VTXApp::get().getEventManager().registerEventReceiverKeyboard( this ); }

		BaseEventReceiverKeyboard ::~BaseEventReceiverKeyboard() { VTXApp::get().getEventManager().unregisterEventReceiverKeyboard( this ); }

	} // namespace Event
} // namespace VTX
