#include "base_event_receiver_sdl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void BaseEventReceiverSDL::_registerEvents()
		{
			if ( _getWindowsID().empty() )
			{
				VTXApp::get().getEventManager().registerEventReceiverSDL( this );
			}
			else
			{
				for ( const ID::VTX_ID & id : _getWindowsID() )
				{
					VTXApp::get().getEventManager().registerEventReceiverSDL( id, this );
				}
			}
		}

		void BaseEventReceiverSDL::_unregisterEvents()
		{
			if ( _getWindowsID().empty() )
			{
				VTXApp::get().getEventManager().unregisterEventReceiverSDL( this );
			}
			else
			{
				for ( const ID::VTX_ID & id : _getWindowsID() )
				{
					VTXApp::get().getEventManager().unregisterEventReceiverSDL( id, this );
				}
			}
		}

	} // namespace Event
} // namespace VTX
