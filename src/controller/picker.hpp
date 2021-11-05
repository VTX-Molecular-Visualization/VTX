#ifndef __VTX_CONTROLLER_PICKER__
#define __VTX_CONTROLLER_PICKER__

#include "base_mouse_controller.hpp"

namespace VTX::Controller
{
	class Picker : public BaseMouseController
	{
	  protected:
		void _handleMouseButtonDownEvent( const QMouseEvent & ) override;
	};
} // namespace VTX::Controller

#endif
