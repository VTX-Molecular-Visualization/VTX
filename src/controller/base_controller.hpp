#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_event_handler.hpp"
#include <SDL2/SDL.h>

namespace VTX
{
	namespace Controller
	{
		class BaseController : public Generic::BaseEventHandler<SDL_Event>
		{
		};
	} // namespace Controller
} // namespace VTX
#endif
