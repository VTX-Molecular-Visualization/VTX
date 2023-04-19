#include "app/event/base_event_receiver_vtx.hpp"
#include "app/core/event/event_manager.hpp"

namespace VTX::Event
{
	BaseEventReceiverVTX ::~BaseEventReceiverVTX()
	{
		for ( const VTX_EVENT & event : _events )
		{
			Core::Event::EventManager::get().unregisterEventReceiverVTX( event, this );
		}

		_events.clear();
	}

	void BaseEventReceiverVTX::_registerEvent( const VTX_EVENT & p_event )
	{
		_events.emplace_back( p_event );
		Core::Event::EventManager::get().registerEventReceiverVTX( p_event, this );
	}

	void BaseEventReceiverVTX::_unregisterEvent( const VTX_EVENT & p_event )
	{
		_events.erase( std::find( _events.begin(), _events.end(), p_event ) );
		Core::Event::EventManager::get().unregisterEventReceiverVTX( p_event, this );
	}
} // namespace VTX::Event
