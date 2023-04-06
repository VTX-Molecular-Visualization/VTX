#include "event_manager.hpp"
#include "controller/base_keyboard_controller.hpp"
#include <util/logger.hpp>
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

		void EventManager::registerEventReceiverKeyboard( BaseEventReceiverKeyboard * const p_receiver )
		{
			_receiversKeyboard.emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverKeyboard( BaseEventReceiverKeyboard * const p_receiver )
		{
			_receiversKeyboard.erase( p_receiver );
		}

		void EventManager::registerEventReceiverMouse( BaseEventReceiverMouse * const p_receiver )
		{
			_receiversMouse.emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverMouse( BaseEventReceiverMouse * const p_receiver )
		{
			_receiversMouse.erase( p_receiver );
		}

		void EventManager::registerEventReceiverWheel( BaseEventReceiverWheel * const p_receiver )
		{
			_receiversWheel.emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverWheel( BaseEventReceiverWheel * const p_receiver )
		{
			_receiversWheel.erase( p_receiver );
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

		void EventManager::fireEventKeyboard( QKeyEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
#ifdef DELAY_EVENTS_QT
			_eventQueueKeyboard.emplace( p_event );
#else
			_flushEventKeyboard( p_event, p_firerer );
#endif
		}

		void EventManager::fireEventMouse( QMouseEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
#ifdef DELAY_EVENTS_QT
			_eventQueueMouse.emplace( p_event );
#else
			_flushEventMouse( p_event, p_firerer );
#endif
		}

		void EventManager::fireEventWheel( QWheelEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
#ifdef DELAY_EVENTS_QT
			_eventQueueWheel.emplace( p_event );
#else
			_flushEventWheel( p_event, p_firerer );
#endif
		}

		void EventManager::update( const float & p_deltaTime )
		{
#ifdef DELAY_EVENTS_QT
			// Input events.
			while ( _eventQueueKeyboard.empty() == false )
			{
				QKeyEvent * const event = _eventQueueKeyboard.front();
				_flushEventKeyboard( event );
				_eventQueueKeyboard.pop();
			}

			while ( _eventQueueMouse.empty() == false )
			{
				QMouseEvent * const event = _eventQueueMouse.front();
				_flushEventMouse( event );
				_eventQueueMouse.pop();
			}

			while ( _eventQueueWheel.empty() == false )
			{
				QWheelEvent * const event = _eventQueueWheel.front();
				_flushEventWheel( event );
				_eventQueueWheel.pop();
			}
#endif
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

		void EventManager::_flushEventKeyboard( QKeyEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
			if ( !_freeze )
			{
				for ( BaseEventReceiverKeyboard * const receiver : _receiversKeyboard )
				{
					if ( receiver->readEventFromInput( p_firerer ) )
					{
						receiver->receiveEvent( *p_event );
					}
				}
			}
		} // namespace Event

		void EventManager::_flushEventMouse( QMouseEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
			if ( !_freeze )
			{
				for ( BaseEventReceiverMouse * const receiver : _receiversMouse )
				{
					if ( receiver->getTargetWidget() == p_firerer )
					{
						receiver->receiveEvent( *p_event );
					}
				}
			}
		}

		void EventManager::_flushEventWheel( QWheelEvent * const p_event, const ID::VTX_ID & p_firerer )
		{
			if ( !_freeze )
			{
				for ( BaseEventReceiverWheel * const receiver : _receiversWheel )
				{
					if ( receiver->getTargetWidget() == p_firerer )
					{
						receiver->receiveEvent( *p_event );
					}
				}
			}
		}

		void EventManager::freezeEvent( const bool p_freeze )
		{
			_freeze = p_freeze;
			clearKeyboardInputEvents();
		}

		void EventManager::clearKeyboardInputEvents() const { Controller::BaseKeyboardController::clear(); }
		void EventManager::clearKeyboardInputEvent( const Qt::Key & p_key ) const
		{
			Controller::BaseKeyboardController::clearKey( p_key );
		}

	} // namespace Event
} // namespace VTX
