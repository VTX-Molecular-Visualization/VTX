#ifndef __VTX_UI_DEBUG_PRINT_INPUTS__
#define __VTX_UI_DEBUG_PRINT_INPUTS__

#include <app/core/input/keys.hpp>

namespace VTX::UI::Debug
{
	class PrintInputs
	{
	  public:
		PrintInputs();

	  private:
		void _logKeyPressed( App::Core::Input::Key p_key ) const;
		void _logKeyReleased( App::Core::Input::Key p_key ) const;
	};
} // namespace VTX::UI::Debug

#endif
