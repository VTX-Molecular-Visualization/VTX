#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include <SDL2/SDL.h>

namespace VTX
{
	namespace Controller
	{
		class BaseController
		{
		  public:
			void handleEvent( const SDL_Event & );
		};
	} // namespace Controller
} // namespace VTX
#endif
