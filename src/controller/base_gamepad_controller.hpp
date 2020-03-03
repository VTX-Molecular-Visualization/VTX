#ifndef __VTX_BASE_GAMEPAD_CONTROLLER__
#define __VTX_BASE_GAMEPAD_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseGamepadController : public BaseController
		{
		  public:
			virtual void receiveEvent( const SDL_Event & ) override {};
		};
	} // namespace Controller
} // namespace VTX
#endif
