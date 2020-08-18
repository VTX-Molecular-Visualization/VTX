#include "event_manager.hpp"
#include "ui/user_interface.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void EventManager::registerEventReceiverSDL( BaseEventReceiverSDL * const p_receiver )
		{
			_receiversSDLGlobal.emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverSDL( BaseEventReceiverSDL * const p_receiver )
		{
			_receiversSDLGlobal.erase( p_receiver );
		}

		void EventManager::registerEventReceiverSDL( const ID::VTX_ID &			  p_windowId,
													 BaseEventReceiverSDL * const p_receiver )
		{
			if ( _receiversSDL.find( p_windowId ) == _receiversSDL.end() )
			{
				_receiversSDL.try_emplace( p_windowId, SetBaseEventReceiverSDLPtr() );
			}

			_receiversSDL.at( p_windowId ).emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverSDL( const ID::VTX_ID &			p_windowId,
													   BaseEventReceiverSDL * const p_receiver )
		{
			_receiversSDL.at( p_windowId ).erase( p_receiver );
		}

		void EventManager::registerEventReceiverVTX( const VTX_EVENT &			  p_event,
													 BaseEventReceiverVTX * const p_receiver )
		{
			if ( _receiversVTX.find( p_event ) == _receiversVTX.end() )
			{
				_receiversVTX.try_emplace( p_event, SetBaseEventReceiverVTXPtr() );
			}

			_receiversVTX.at( p_event ).emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverVTX( const VTX_EVENT &			p_event,
													   BaseEventReceiverVTX * const p_receiver )
		{
			_receiversVTX.at( p_event ).erase( p_receiver );
		}

		void EventManager::fireEvent( VTXEvent * const p_event )
		{
			_lock();
#ifdef DELAY_EVENTS
			_eventQueue.emplace( p_event );
#else
			_flushVTXEvent( p_event );
#endif
			_unlock();
		}

		void EventManager::update( const double & p_deltaTime )
		{
			// SDL.
			SDL_Event event;
			while ( VTXApp::get().getUI().getEvent( event ) )
			{
				switch ( event.type )
				{
				case SDL_QUIT:
				{
					VTXApp::get().stop();
					break;
				}
				case SDL_WINDOWEVENT:
				{
					_handlerWindowEvent( event.window );
					break;
				}
				}

				// Propagate to receivers.
				for ( Event::BaseEventReceiverSDL * const receiver : _receiversSDLGlobal )
				{
					receiver->receiveEvent( event );
				}

				// Then other
				const ID::VTX_ID & focused = VTXApp::get().getUI().getFocusedWindow();
				const ID::VTX_ID & hovered = VTXApp::get().getUI().getHoveredWindow();

				// If mouse or key up, propagate to all.
				if ( event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONUP )
				{
					for ( const std::pair<ID::VTX_ID, SetBaseEventReceiverSDLPtr> & receiverSDL : _receiversSDL )
					{
						for ( BaseEventReceiverSDL * const receiver : receiverSDL.second )
						{
							receiver->receiveEvent( event );
						}
					}
				}
				// Else propagate to subscribers.
				else if ( _receiversSDL.find( hovered ) != _receiversSDL.end() )
				{
					for ( BaseEventReceiverSDL * const receiver : _receiversSDL.at( hovered ) )
					{
						receiver->receiveEvent( event );
					}
				}
			}

			// VTX.
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

		void EventManager::_handlerWindowEvent( const SDL_WindowEvent & p_event )
		{
			switch ( p_event.event )
			{
			case SDL_WINDOWEVENT_CLOSE: VTXApp::get().stop(); break;
			}
		}

	} // namespace Event
} // namespace VTX
