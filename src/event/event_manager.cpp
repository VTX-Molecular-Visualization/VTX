#include "event_manager.hpp"
#include "vtx_app.hpp"

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

		void EventManager::registerEventReceiverVTX( const VTX_EVENT & p_event, BaseEventReceiverVTX * const p_receiver )
		{
			if ( _receiversVTX.find( p_event ) == _receiversVTX.end() )
			{
				_receiversVTX.try_emplace( p_event, SetBaseEventReceiverVTXPtr() );
			}

			_receiversVTX.at( p_event ).emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverVTX( const VTX_EVENT & p_event, BaseEventReceiverVTX * const p_receiver ) { _receiversVTX.at( p_event ).erase( p_receiver ); }

		void EventManager::registerEventReceiverKeyboard( BaseEventReceiverKeyboard * const p_receiver ) { _receiversKeyboard.emplace( p_receiver ); }

		void EventManager::unregisterEventReceiverKeyboard( BaseEventReceiverKeyboard * const p_receiver ) { _receiversKeyboard.erase( p_receiver ); }

		void EventManager::registerEventReceiverMouse( BaseEventReceiverMouse * const p_receiver ) { _receiversMouse.emplace( p_receiver ); }

		void EventManager::unregisterEventReceiverMouse( BaseEventReceiverMouse * const p_receiver ) { _receiversMouse.erase( p_receiver ); }

		void EventManager::registerEventReceiverWheel( BaseEventReceiverWheel * const p_receiver ) { _receiversWheel.emplace( p_receiver ); }

		void EventManager::unregisterEventReceiverWheel( BaseEventReceiverWheel * const p_receiver ) { _receiversWheel.erase( p_receiver ); }

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

		void EventManager::fireEventKeyboard( QKeyEvent * const p_event ) { _eventQueueKeyboard.emplace( *p_event ); }

		void EventManager::fireEventMouse( QMouseEvent * const p_event ) { _eventQueueMouse.emplace( *p_event ); }

		void EventManager::fireEventWheel( QWheelEvent * const p_event ) { _eventQueueWheel.emplace( *p_event ); }

		void EventManager::update( const float & p_deltaTime )
		{
			// Input events.
			while ( _eventQueueKeyboard.empty() == false )
			{
				const QKeyEvent & event = _eventQueueKeyboard.front();
				for ( Event::BaseEventReceiverKeyboard * const receiver : _receiversKeyboard )
				{
					receiver->receiveEvent( event );
				}
				_eventQueueKeyboard.pop();
			}

			while ( _eventQueueMouse.empty() == false )
			{
				const QMouseEvent & event = _eventQueueMouse.front();
				for ( Event::BaseEventReceiverMouse * const receiver : _receiversMouse )
				{
					receiver->receiveEvent( event );
				}
				_eventQueueMouse.pop();
			}

			while ( _eventQueueWheel.empty() == false )
			{
				const QWheelEvent & event = _eventQueueWheel.front();
				for ( Event::BaseEventReceiverWheel * const receiver : _receiversWheel )
				{
					receiver->receiveEvent( event );
				}
				_eventQueueWheel.pop();
			}

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
			if ( _receiversVTX.find( p_event->name ) != _receiversVTX.end() )
			{
				for ( BaseEventReceiverVTX * const receiver : _receiversVTX.at( p_event->name ) )
				{
					receiver->receiveEvent( *p_event );
				}
			}
			delete p_event;
		}

	} // namespace Event
} // namespace VTX
