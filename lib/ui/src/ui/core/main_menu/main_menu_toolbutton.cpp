#include "ui/core/main_menu/main_menu_toolbutton.hpp"

namespace VTX::UI::Core::MainMenu
{
	MainMenuToolButton::MainMenuToolButton() {}

	void MainMenuToolButton::showActiveFeedback( const bool p_activate ) { _hasActiveFeedback = p_activate; }
} // namespace VTX::UI::Core::MainMenu
