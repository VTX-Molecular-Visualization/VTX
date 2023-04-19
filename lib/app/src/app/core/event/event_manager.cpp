#include "app/core/event/event_manager.hpp"
#include "app/event/base_event_receiver_vtx.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::Core::Event
{
	EventManager::~EventManager()
	{
		while ( _eventQueueVTX.empty() == false )
		{
			VTX::Event::VTXEvent * const event = _eventQueueVTX.front();
			_eventQueueVTX.pop();
			delete event;
		}
	}

	void EventManager::registerEventReceiverVTX( const VTX::Event::VTX_EVENT &			  p_event,
												 VTX::Event::BaseEventReceiverVTX * const p_receiver )
	{
		_receiversVTX[ p_event ].emplace( p_receiver );
	}

	void EventManager::unregisterEventReceiverVTX( const VTX::Event::VTX_EVENT &			p_event,
												   VTX::Event::BaseEventReceiverVTX * const p_receiver )
	{
		_receiversVTX[ p_event ].erase( p_receiver );
	}

	void EventManager::fireEventVTX( VTX::Event::VTXEvent * const p_event )
	{
#ifdef DELAY_EVENTS
		_lock();
		_eventQueueVTX.emplace( p_event );
		_unlock();
#else
		_flushVTXEvent( p_event );
#endif
	}

	void EventManager::update( const float & p_deltaTime )
	{
		// VTX events.
		while ( _eventQueueVTX.empty() == false )
		{
			_lock();
			VTX::Event::VTXEvent * const event = _eventQueueVTX.front();
			_unlock();
			_flushVTXEvent( event );
			_lock();
			_eventQueueVTX.pop();
			_unlock();
		}
	}

	void EventManager::_flushVTXEvent( VTX::Event::VTXEvent * const p_event )
	{
		if ( !_freeze )
		{
			if ( _receiversVTX.find( p_event->name ) != _receiversVTX.end() )
			{
				for ( VTX::Event::BaseEventReceiverVTX * const receiver : _receiversVTX[ p_event->name ] )
				{
					receiver->receiveEvent( *p_event );
				}
			}
		}

		delete p_event;
	}

	void EventManager::freezeEvent( const bool p_freeze ) { _freeze = p_freeze; }
} // namespace VTX::Core::Event
