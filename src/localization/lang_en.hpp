#ifndef __VTX_LANG_EN__
#define __VTX_LANG_EN__

#include <map>
#include <string>

namespace VTX
{
	namespace Localization
	{
		static std::map<std::string, const char *> values = {

			// Main menu.
			{ "MainMenu.Menu", "Menu" },
			{ "MainMenu.Menu.Quit", "Quit" },
			{ "MainMenu.Import", "Import" },
			{ "MainMenu.Import.MMTF", "MMTF" },
			{ "MainMenu.Display", "Display" },
			{ "MainMenu.Display.Scene", "Scene" },
			{ "MainMenu.Display.Selection", "Selection" },
			{ "MainMenu.Display.Console", "Console" },
			{ "MainMenu.Settings", "Options" },
			{ "MainMenu.Settings.Render", "Render mode" },
			{ "MainMenu.Settings.Shading", "Shading" },
			{ "MainMenu.Settings.AA", "AA" },
			{ "MainMenu.Settings.SSAO", "SSAO" },
			{ "MainMenu.Settings.BLUR", "BLUR" },

			// Console.
			{ "Console.Console", "Console" },

			// Scene.
			{ "Scene.Scene", "Scene" },

			// Selection.
			{ "Selection.Selection", "Selection" }
		};

	} // namespace Localization
} // namespace VTX
#endif
