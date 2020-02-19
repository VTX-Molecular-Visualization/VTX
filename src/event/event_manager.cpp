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

		void EventManager::fireEvent( const VTX_EVENT & p_event, void * const p_arg )
		{
			if ( _receiversVTX.find( p_event ) != _receiversVTX.end() )
			{
				for ( BaseEventReceiverVTX * const receiver : _receiversVTX.at( p_event ) )
				{
					receiver->receiveEvent( p_event, p_arg );
				}
			}
		}

		void EventManager::update( const double p_deltaTime )
		{
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
						receiver->receiveEvent( event );
					}
				}
			}
		}

	} // namespace Event
} // namespace VTX
