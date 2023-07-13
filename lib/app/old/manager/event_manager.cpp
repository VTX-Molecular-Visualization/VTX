#include "app/old/manager/event_manager.hpp"
#include "app/old/core/event/base_event_receiver_vtx.hpp"
#include "app/old/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App::Old::Manager
{
	EventManager::~EventManager()
	{
		while ( _eventQueueVTX.empty() == false )
		{
			VTX::App::Old::Core::Event::VTXEvent * const event = _eventQueueVTX.front();
			_eventQueueVTX.pop();
			delete event;
		}
	}

	void EventManager::registerEventReceiverVTX( const VTX::App::Old::Core::Event::VTX_EVENT &			 p_event,
												 VTX::App::Old::Core::Event::BaseEventReceiverVTX * const p_receiver )
	{
		_receiversVTX[ p_event ].emplace( p_receiver );
	}

	void EventManager::unregisterEventReceiverVTX( const VTX::App::Old::Core::Event::VTX_EVENT &			   p_event,
												   VTX::App::Old::Core::Event::BaseEventReceiverVTX * const p_receiver )
	{
		_receiversVTX[ p_event ].erase( p_receiver );
	}

	void EventManager::fireEventVTX( VTX::App::Old::Core::Event::VTXEvent * const p_event )
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
			VTX::App::Old::Core::Event::VTXEvent * const event = _eventQueueVTX.front();
			_unlock();
			_flushVTXEvent( event );
			_lock();
			_eventQueueVTX.pop();
			_unlock();
		}
	}

	void EventManager::_flushVTXEvent( VTX::App::Old::Core::Event::VTXEvent * const p_event )
	{
		if ( !_freeze )
		{
			if ( _receiversVTX.find( p_event->name ) != _receiversVTX.end() )
			{
				for ( VTX::App::Old::Core::Event::BaseEventReceiverVTX * const receiver : _receiversVTX[ p_event->name ] )
				{
					receiver->receiveEvent( *p_event );
				}
			}
		}

		delete p_event;
	}

	void EventManager::freezeEvent( const bool p_freeze ) { _freeze = p_freeze; }
} // namespace VTX::App::Old::Manager