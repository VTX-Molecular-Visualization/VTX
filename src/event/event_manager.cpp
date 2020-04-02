#include "event_manager.hpp"
#include "action/resize.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Event
	{
		void EventManager::registerEventReceiverSDL( BaseEventReceiverSDL * const p_receiver )
		{
			_receiversSDL.emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverSDL( BaseEventReceiverSDL * const p_receiver )
		{
			_receiversSDL.erase( p_receiver );
		}

		void EventManager::registerEventReceiverVTX( const VTX_EVENT &			  p_event,
													 BaseEventReceiverVTX * const p_receiver )
		{
			if ( _receiversVTX.find( p_event ) == _receiversVTX.end() )
			{ _receiversVTX.try_emplace( p_event, SetBaseEventReceiverVTXPtr() ); }

			_receiversVTX.at( p_event ).emplace( p_receiver );
		}

		void EventManager::unregisterEventReceiverVTX( const VTX_EVENT &			p_event,
													   BaseEventReceiverVTX * const p_receiver )
		{
			_receiversVTX.at( p_event ).erase( p_receiver );
		}

		void EventManager::_flushEvent( VTXEvent * p_event )
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

				// Propagate.
				if ( ImGui::IsAnyWindowFocused() == false )
				{
					for ( Event::BaseEventReceiverSDL * const receiver : _receiversSDL )
					{
						receiver->receiveEvent( event );
					}
				}
			}

			// VTX.
			while ( _eventQueue.empty() == false )
			{
				_flushEvent( _eventQueue.front() );
				_eventQueue.pop();
			}
		}

		void EventManager::_handlerWindowEvent( const SDL_WindowEvent & p_event )
		{
			switch ( p_event.event )
			{
			case SDL_WINDOWEVENT_CLOSE: VTXApp::get().stop(); break;
			case SDL_WINDOWEVENT_RESIZED: VTX_ACTION( new Action::Resize( p_event.data1, p_event.data2 ) ); break;
			}
		}

	} // namespace Event
} // namespace VTX
