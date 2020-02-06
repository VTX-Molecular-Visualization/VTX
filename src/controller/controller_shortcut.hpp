#ifndef __VTX_CONTROLLER_SHORTCUT__
#define __VTX_CONTROLLER_SHORTCUT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../id.hpp"
#include "base_keyboard_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class ControllerShortcut : public BaseKeyboardController
		{
		  public:
			explicit ControllerShortcut() {}

		  protected:
			virtual void _handleKeyPressedEvent( const SDL_Scancode & ) override;

			virtual std::string getName() const override { return ID::Controller::SHORTCUT; }

		  private:
		};
	} // namespace Controller
} // namespace VTX
#endif
