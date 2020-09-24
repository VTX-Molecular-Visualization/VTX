#include "base_event_receiver_vtx.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverVTX ::~BaseEventReceiverVTX()
		{
			for ( const VTX_EVENT & event : _events )
			{
				VTXApp::get().getEventManager().unregisterEventReceiverVTX( event, this );
			}
			_events.clear();
		}

		void BaseEventReceiverVTX::_registerEvent( const VTX_EVENT & p_event )
		{
			_events.emplace_back( p_event );
			VTXApp::get().getEventManager().registerEventReceiverVTX( p_event, this );
		}

		void BaseEventReceiverVTX::_unregisterEvent( const VTX_EVENT & p_event )
		{
			_events.erase( std::find( _events.begin(), _events.end(), p_event ) );
			VTXApp::get().getEventManager().unregisterEventReceiverVTX( p_event, this );
		}
	} // namespace Event
} // namespace VTX
