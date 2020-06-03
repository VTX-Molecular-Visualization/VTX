#include "event_manager.hpp"
#include "action/resize.hpp"
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

		void EventManager::fireEvent( VTXEvent * const p_event ) { _eventQueue.push( p_event ); }

		void EventManager::update( const double p_deltaTime )
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

				const ID::VTX_ID & id = VTXApp::get().getUI().getFocusedWindow();
				if ( _receiversSDL.find( id ) != _receiversSDL.end() )
				{
					for ( BaseEventReceiverSDL * const receiver : _receiversSDL.at( id ) )
					{
						receiver->receiveEvent( event );
					}
				}
			}

			// VTX.
			while ( _eventQueue.empty() == false )
			{
				VTXEvent * const event = _eventQueue.front();
				if ( _receiversVTX.find( event->name ) != _receiversVTX.end() )
				{
					for ( BaseEventReceiverVTX * const receiver : _receiversVTX.at( event->name ) )
					{
						receiver->receiveEvent( *event );
					}
				}
				delete event;
				_eventQueue.pop();
			}
		}

		void EventManager::_handlerWindowEvent( const SDL_WindowEvent & p_event )
		{
			switch ( p_event.event )
			{
			case SDL_WINDOWEVENT_CLOSE:
				VTXApp::get().stop();
				break;
				// case SDL_WINDOWEVENT_RESIZED: VTX_ACTION( new Action::Resize( p_event.data1, p_event.data2 ) );
				// break;
			}
		}

	} // namespace Event
} // namespace VTX
