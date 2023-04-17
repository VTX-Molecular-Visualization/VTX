#include "app/old_app/event/base_event_receiver_vtx.hpp"
#include "app/old_app/event/event_manager.hpp"

namespace VTX
{
	namespace Event
	{
		BaseEventReceiverVTX ::~BaseEventReceiverVTX()
		{
			for ( const VTX_EVENT & event : _events )
			{
				EventManager::get().unregisterEventReceiverVTX( event, this );
			}
			_events.clear();
		}

		void BaseEventReceiverVTX::_registerEvent( const VTX_EVENT & p_event )
		{
			_events.emplace_back( p_event );
			EventManager::get().registerEventReceiverVTX( p_event, this );
		}

		void BaseEventReceiverVTX::_unregisterEvent( const VTX_EVENT & p_event )
		{
			_events.erase( std::find( _events.begin(), _events.end(), p_event ) );
			EventManager::get().unregisterEventReceiverVTX( p_event, this );
		}
	} // namespace Event
} // namespace VTX
