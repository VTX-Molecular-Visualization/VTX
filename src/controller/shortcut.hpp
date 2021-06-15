#ifndef __VTX_CONTROLLER_SHORTCUT__
#define __VTX_CONTROLLER_SHORTCUT__

#include "base_keyboard_controller.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Controller
	{
		class Shortcut : public BaseKeyboardController
		{
		  public:
			Shortcut() = default;

		  protected:
			void _handleKeyDownEvent( const ScanCode & ) override;
			void update( const float & ) override {}

		  private:
		};
	} // namespace Controller
} // namespace VTX
#endif
