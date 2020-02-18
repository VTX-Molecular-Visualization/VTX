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

		void EventManager::update( const double p_deltaTime )
		{
			SDL_Event event;
			while ( VTXApp::get().getUI().pollEvent( event ) )
			{
				VTXApp::get().getUI().processEvent( event );

				switch ( event.type )
				{
				case SDL_QUIT: VTXApp::get().stop(); return;
				case SDL_WINDOWEVENT:
					if ( event.window.event == SDL_WINDOWEVENT_CLOSE )
					{
						VTXApp::get().stop();
						return;
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
