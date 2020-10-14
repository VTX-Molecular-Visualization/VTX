#include "base_event_receiver_mouse.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverMouse ::BaseEventReceiverMouse() { VTXApp::get().getEventManager().registerEventReceiverMouse( this ); }

		BaseEventReceiverMouse ::~BaseEventReceiverMouse() { VTXApp::get().getEventManager().unregisterEventReceiverMouse( this ); }

	} // namespace Event
} // namespace VTX
