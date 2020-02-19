#include "base_event_receiver_vtx.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void BaseEventReceiverVTX::_registerEvents()
		{
			for ( const VTX_EVENT & event : _getEvents() )
			{
				VTXApp::get().getEventManager().registerEventReceiverVTX( event, this );
			}
		}

		void BaseEventReceiverVTX::_unregisterEvents()
		{
			for ( const VTX_EVENT & event : _getEvents() )
			{
				VTXApp::get().getEventManager().unregisterEventReceiverVTX( event, this );
			}
		}

	} // namespace Event
} // namespace VTX
