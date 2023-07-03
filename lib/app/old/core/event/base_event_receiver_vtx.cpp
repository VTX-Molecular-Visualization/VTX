#include "app/old/core/event/base_event_receiver_vtx.hpp"
#include "app/old/event.hpp"
#include "app/old/manager/event_manager.hpp"

namespace VTX::App::Old::Core::Event
{
	BaseEventReceiverVTX ::~BaseEventReceiverVTX()
	{
		for ( const VTX_EVENT & event : _events )
		{
			Manager::EventManager::get().unregisterEventReceiverVTX( event, this );
		}

		_events.clear();
	}

	void BaseEventReceiverVTX::_registerEvent( const VTX_EVENT & p_event )
	{
		_events.emplace_back( p_event );
		Manager::EventManager::get().registerEventReceiverVTX( p_event, this );
	}

	void BaseEventReceiverVTX::_unregisterEvent( const VTX_EVENT & p_event )
	{
		_events.erase( std::find( _events.begin(), _events.end(), p_event ) );
		Manager::EventManager::get().unregisterEventReceiverVTX( p_event, this );
	}
} // namespace VTX::App::Old::Core::Event
