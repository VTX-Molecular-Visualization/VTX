#include "main_window_controller.hpp"

namespace VTX::UI::QT::Controller
{
	void MainWindowController::receiveEvent( const QKeyEvent & p_event )
	{
		if ( isActive() == false )
			return;

		updateKeyboardBuffer( p_event );
	}

} // namespace VTX::UI::QT::Controller
