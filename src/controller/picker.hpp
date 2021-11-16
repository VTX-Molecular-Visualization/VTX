#ifndef __VTX_CONTROLLER_PICKER__
#define __VTX_CONTROLLER_PICKER__

#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"

namespace VTX::Controller
{
	class Picker : public BaseMouseController, public BaseKeyboardController
	{
	  public:
		void update( const float & p_deltaTime ) override;

	  protected:
		void _onMouseLeftClick( const uint p_x, const uint p_y ) override;
	};
} // namespace VTX::Controller

#endif
