#include "event_manager.hpp"
#include "old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX
{
	namespace Event
	{
		EventManager::~EventManager()
		{
			while ( _eventQueueVTX.empty() == false )
			{
				VTXEvent * const event = _eventQueueVTX.front();
				_eventQueueVTX.pop();
				delete event;
			}
		}

		void EventManager::registerEventReceiverVTX( const VTX_EVENT &			  p_event,
													 BaseEventReceiverVTX * const p_receiver )
		{
			_receiversVTX[ p_event ].emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverVTX( const VTX_EVENT &			p_event,
													   BaseEventReceiverVTX * const p_receiver )
		{
			_receiversVTX[ p_event ].erase( p_receiver );
		}

		void EventManager::fireEventVTX( VTXEvent * const p_event )
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
				VTXEvent * const event = _eventQueueVTX.front();
				_unlock();
				_flushVTXEvent( event );
				_lock();
				_eventQueueVTX.pop();
				_unlock();
			}
		}

		void EventManager::_flushVTXEvent( VTXEvent * const p_event )
		{
			if ( !_freeze )
			{
				if ( _receiversVTX.find( p_event->name ) != _receiversVTX.end() )
				{
					for ( BaseEventReceiverVTX * const receiver : _receiversVTX[ p_event->name ] )
					{
						receiver->receiveEvent( *p_event );
					}
				}
			}

			delete p_event;
		}

		void EventManager::freezeEvent( const bool p_freeze ) { _freeze = p_freeze; }
	} // namespace Event
} // namespace VTX
