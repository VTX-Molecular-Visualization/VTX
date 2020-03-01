#include "event_manager.hpp"
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

		void EventManager::flushEvent( VTXEvent * p_event )
		{
			if ( _receiversVTX.find( p_event->name ) != _receiversVTX.end() )
			{
				for ( BaseEventReceiverVTX * const receiver : _receiversVTX.at( p_event->name ) )
				{
					receiver->receiveEvent( p_event );
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
					return;
				}
				case SDL_WINDOWEVENT:
				{
					if ( event.window.event == SDL_WINDOWEVENT_CLOSE )
					{
						VTXApp::get().stop();
						return;
					}
				}
				}

				// Propagate.
				if ( ImGui::IsAnyWindowFocused() == false )
				{
					for ( Event::BaseEventReceiverSDL * const receiver : _receiversSDL )
					{
						receiver->receiveEvent( &event );
					}
				}
			}

			// VTX.
			while ( _eventQueue.empty() == false )
			{
				flushEvent( _eventQueue.front() );
				_eventQueue.pop();
			}
		}

	} // namespace Event
} // namespace VTX
