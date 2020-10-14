#include "base_event_receiver_wheel.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverWheel ::BaseEventReceiverWheel() { VTXApp::get().getEventManager().registerEventReceiverWheel( this ); }

		BaseEventReceiverWheel ::~BaseEventReceiverWheel() { VTXApp::get().getEventManager().unregisterEventReceiverWheel( this ); }

	} // namespace Event
} // namespace VTX
