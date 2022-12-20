#ifndef __VTX_UI_CORE_MAIN_WINDOW__
#define __VTX_UI_CORE_MAIN_WINDOW__

#include "main_menu/main_menu_bar.hpp"

namespace VTX::UI::Core
{
	enum class WindowMode
	{
		Fullscreen,
		Windowed,
		Maximized,
		Minimized,
	};

	class MainWindow
	{
	  public:
		MainWindow();
		~MainWindow();

		virtual MainMenu::MainMenuBar & getMainMenu() = 0;
	};

} // namespace VTX::UI::Core

#endif
