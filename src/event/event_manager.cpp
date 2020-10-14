#include "event_manager.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
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
			_lock();
#ifdef DELAY_EVENTS
			_eventQueue.emplace( p_event );
#else
			_flushVTXEvent( p_event );
#endif
			_unlock();
		}

		void EventManager::fireEventKeyboard( const QKeyEvent & p_event )
		{
			for ( Event::BaseEventReceiverKeyboard * const receiver : _receiversKeyboard )
			{
				receiver->receiveEvent( p_event );
			}
		}

		void EventManager::fireEventMouse( const QMouseEvent & p_event )
		{
			for ( Event::BaseEventReceiverMouse * const receiver : _receiversMouse )
			{
				receiver->receiveEvent( p_event );
			}
		}

		void EventManager::fireEventWheel( const QWheelEvent & p_event )
		{
			for ( Event::BaseEventReceiverWheel * const receiver : _receiversWheel )
			{
				receiver->receiveEvent( p_event );
			}
		}

		void EventManager::update( const double & p_deltaTime )
		{
			_lock();
			while ( _eventQueue.empty() == false )
			{
				VTXEvent * const event = _eventQueue.front();
				_flushVTXEvent( event );
				_eventQueue.pop();
			}
			_unlock();
		}

		void EventManager::_flushVTXEvent( VTXEvent * p_event )
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
