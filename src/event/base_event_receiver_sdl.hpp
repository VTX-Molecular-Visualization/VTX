#ifndef __VTX_BASE_EVENT_RECEIVER_SDL__
#define __VTX_BASE_EVENT_RECEIVER_SDL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver.hpp"
//#include "vtx_app.hpp"
#include <SDL2/SDL.h>

// TODO:
// Finir ça et rebrancher SDL events dans l'eventManager (BaseUpdatable).
// Peut être ajouter BaseInitializable pour register les events.
// Faire les events VTX.
// Voir si on change le pattern notifier ou pas.

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverSDL : public BaseEventReceiver<SDL_Event>
		{
		  public:
			virtual void registerEvents()
			{ /*VTXApp::get().getEventManager().registerEventReceiverSDL( this );*/
			}
		};
	} // namespace Event
} // namespace VTX
#endif
