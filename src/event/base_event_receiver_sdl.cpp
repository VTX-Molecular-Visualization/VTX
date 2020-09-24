#include "base_event_receiver_sdl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void BaseEventReceiverSDL::_registerEvents()
		{
			if ( _getUIItems().empty() )
			{
				VTXApp::get().getEventManager().registerEventReceiverSDL( this );
			}
			else
			{
				for ( const ID::VTX_ID & id : _getUIItems() )
				{
					VTXApp::get().getEventManager().registerEventReceiverSDL( id, this );
				}
			}
		}

		void BaseEventReceiverSDL::_unregisterEvents()
		{
			if ( _getUIItems().empty() )
			{
				VTXApp::get().getEventManager().unregisterEventReceiverSDL( this );
			}
			else
			{
				for ( const ID::VTX_ID & id : _getUIItems() )
				{
					VTXApp::get().getEventManager().unregisterEventReceiverSDL( id, this );
				}
			}
		}

		void BaseEventReceiverSDL::_registerEvent( const VTX_EVENT & ) {}

		void BaseEventReceiverSDL::_unregisterEvent( const VTX_EVENT & ) {}

	} // namespace Event
} // namespace VTX
