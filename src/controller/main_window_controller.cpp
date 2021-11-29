#include "main_window_controller.hpp"

namespace VTX::Controller
{
	void MainWindowController::receiveEvent( const QKeyEvent & p_event )
	{
		if ( isActive() == false )
			return;

		updateKeyboardBuffer( p_event );
	}

} // namespace VTX::Controller
