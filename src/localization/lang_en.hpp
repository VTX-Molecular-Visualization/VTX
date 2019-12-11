#ifndef __VTX_LANG_EN__
#define __VTX_LANG_EN__

#ifdef _MSC_VER
#pragma once
#endif

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
			{ "MainMenu.Display.CloseAll", "Close all" },
			{ "MainMenu.Display.ShowAll", "Show all" },
			{ "MainMenu.Settings", "Options" },
			{ "MainMenu.Settings.Theme", "Theme" },
			{ "MainMenu.Settings.SymbolDisplay", "Symbol display" },
			{ "MainMenu.Settings.Representation", "Representation" },
			{ "MainMenu.Settings.Shading", "Shading" },
			{ "MainMenu.Settings.AA", "AA" },
			{ "MainMenu.Settings.SSAO", "SSAO" },

			// Console.
			{ "Console.Console", "Console" },

			// Scene.
			{ "Scene.Scene", "Scene" },
			{ "Scene.Performance", "Performance" },

			// Selection.
			{ "Selection.Selection", "Selection" },

			// Views.
			{ "View.Molecule.Data", "Data" }
		};

	} // namespace Localization
} // namespace VTX
#endif
