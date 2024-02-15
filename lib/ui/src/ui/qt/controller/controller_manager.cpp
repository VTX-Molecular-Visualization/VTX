// #include "ui/qt/controller/controller_manager.hpp"
// #include <util/logger.hpp>
//
// namespace VTX::UI::QT::Controller
//{
//	ControllerManager::~ControllerManager() {}
//
//	void ControllerManager::registerEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const p_receiver )
//	{
//		_receiversKeyboard.emplace( p_receiver );
//	}
//
//	void ControllerManager::unregisterEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const p_receiver )
//	{
//		_receiversKeyboard.erase( p_receiver );
//	}
//
//	void ControllerManager::registerEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const p_receiver )
//	{
//		_receiversMouse.emplace( p_receiver );
//	}
//
//	void ControllerManager::unregisterEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const p_receiver )
//	{
//		_receiversMouse.erase( p_receiver );
//	}
//
//	void ControllerManager::registerEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const p_receiver )
//	{
//		_receiversWheel.emplace( p_receiver );
//	}
//
//	void ControllerManager::unregisterEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const p_receiver )
//	{
//		_receiversWheel.erase( p_receiver );
//	}
//
//	void ControllerManager::fireEventKeyboard( QKeyEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
//		_flushEventKeyboard( p_event, p_firerer );
//	}
//
//	void ControllerManager::fireEventMouse( QMouseEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
//		_flushEventMouse( p_event, p_firerer );
//	}
//
//	void ControllerManager::fireEventWheel( QWheelEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
// #ifdef DELAY_EVENTS_QT
//		_eventQueueWheel.emplace( p_event );
// #else
//		_flushEventWheel( p_event, p_firerer );
// #endif
//	}
//
//	void ControllerManager::_flushEventKeyboard( QKeyEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
//		if ( !_freeze )
//		{
//			for ( VTX::Event::BaseEventReceiverKeyboard * const receiver : _receiversKeyboard )
//			{
//				if ( receiver->readEventFromInput( p_firerer ) )
//				{
//					receiver->receiveEvent( *p_event );
//				}
//			}
//		}
//	} // namespace Event
//
//	void ControllerManager::_flushEventMouse( QMouseEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
//		if ( !_freeze )
//		{
//			for ( VTX::Event::BaseEventReceiverMouse * const receiver : _receiversMouse )
//			{
//				if ( receiver->getTargetWidget() == p_firerer )
//				{
//					receiver->receiveEvent( *p_event );
//				}
//			}
//		}
//	}
//
//	void ControllerManager::_flushEventWheel( QWheelEvent * const p_event, const App::Old::VTX_ID & p_firerer )
//	{
//		if ( !_freeze )
//		{
//			for ( VTX::Event::BaseEventReceiverWheel * const receiver : _receiversWheel )
//			{
//				if ( receiver->getTargetWidget() == p_firerer )
//				{
//					receiver->receiveEvent( *p_event );
//				}
//			}
//		}
//	}
//
//	void ControllerManager::freezeEvent( const bool p_freeze )
//	{
//		_freeze = p_freeze;
//		clearKeyboardInputEvents();
//	}
//
//	void ControllerManager::clearKeyboardInputEvents() const { Controller::BaseKeyboardController::clear(); }
//	void ControllerManager::clearKeyboardInputEvent( const Qt::Key & p_key ) const
//	{
//		Controller::BaseKeyboardController::clearKey( p_key );
//	}
//
// } // namespace VTX::UI::Qt::Controller
