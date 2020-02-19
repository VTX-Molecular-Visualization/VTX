#include "base_event_receiver_sdl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void BaseEventReceiverSDL::_registerEvents()
		{
			VTXApp::get().getEventManager().registerEventReceiverSDL( this );
		}

		void BaseEventReceiverSDL::_unregisterEvents()
		{
			VTXApp::get().getEventManager().unregisterEventReceiverSDL( this );
		}

	} // namespace Event
} // namespace VTX
