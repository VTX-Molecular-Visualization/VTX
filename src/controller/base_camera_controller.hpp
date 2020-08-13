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
		class BaseCameraController : virtual public BaseController
		{
		  public:
			virtual ~BaseCameraController() = default;
			// TODO: recenter only on visible objects
			virtual void reset() = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
