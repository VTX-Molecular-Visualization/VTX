#include "ui/old_ui/event/event_manager.hpp"
#include "ui/old_ui/controller/base_keyboard_controller.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::UI::Event
{
	EventManager::~EventManager() {}

	void EventManager::registerEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const p_receiver )
	{
		_receiversKeyboard.emplace( p_receiver );
	}

	void EventManager::unregisterEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const p_receiver )
	{
		_receiversKeyboard.erase( p_receiver );
	}

	void EventManager::registerEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const p_receiver )
	{
		_receiversMouse.emplace( p_receiver );
	}

	void EventManager::unregisterEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const p_receiver )
	{
		_receiversMouse.erase( p_receiver );
	}

	void EventManager::registerEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const p_receiver )
	{
		_receiversWheel.emplace( p_receiver );
	}

	void EventManager::unregisterEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const p_receiver )
	{
		_receiversWheel.erase( p_receiver );
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
	}

	void EventManager::_flushEventKeyboard( QKeyEvent * const p_event, const ID::VTX_ID & p_firerer )
	{
		if ( !_freeze )
		{
			for ( VTX::Event::BaseEventReceiverKeyboard * const receiver : _receiversKeyboard )
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
			for ( VTX::Event::BaseEventReceiverMouse * const receiver : _receiversMouse )
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
			for ( VTX::Event::BaseEventReceiverWheel * const receiver : _receiversWheel )
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

} // namespace VTX::UI::Event
