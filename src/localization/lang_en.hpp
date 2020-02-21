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
			{ "MainMenu.Menu.New", "New" },
			{ "MainMenu.Menu.Open", "Open" },
			
			{ "MainMenu.Menu.Open.ChooseFile", "Choose file" },
			{ "MainMenu.Menu.Quit", "Quit" },
			{ "MainMenu.Import", "Import" },
			{ "MainMenu.Import.MMTF", "MMTF" },
			{ "MainMenu.Display", "Display" },
			{ "MainMenu.Display.Scene", "Scene" },
			{ "MainMenu.Display.Inspector", "Inspector" },
			{ "MainMenu.Display.Console", "Console" },
			{ "MainMenu.Display.CameraEditor", "Camera editor" },
			{ "MainMenu.Display.CloseAll", "Close all" },
			{ "MainMenu.Display.ShowAll", "Show all" },
			{ "MainMenu.Import", "Import" },
			{ "MainMenu.Import.Path", "Path" },
			{ "MainMenu.Export", "Export" },
			{ "MainMenu.Export.Snapshot", "Snapshot" },
			{ "MainMenu.Export.Path", "Path" },
			{ "MainMenu.Export.Video", "Video" },
			{ "MainMenu.Settings", "Options" },
			{ "MainMenu.Settings.Theme", "Theme" },
			{ "MainMenu.Settings.SymbolDisplay", "Symbol display" },
			{ "MainMenu.Settings.Rendering", "Rendering" },
			{ "MainMenu.Settings.Representation", "Representation" },
			{ "MainMenu.Settings.ColorMode", "Color mode" },
			{ "MainMenu.Settings.Shading", "Shading" },
			{ "MainMenu.Settings.AA", "AA" },
			{ "MainMenu.Settings.SSAO", "SSAO" },
			{ "MainMenu.Settings.AORadius", "AO radius" },
			{ "MainMenu.Settings.AOIntensity", "AO intensity" },
			{ "MainMenu.Settings.AOBlurSize", "AO blur size" },
			{ "MainMenu.Settings.AutoRotateXSpeed", "X auto rotation speed" },
			{ "MainMenu.Settings.AutoRotateYSpeed", "Y auto rotation speed" },
			{ "MainMenu.Settings.AutoRotateZSpeed", "Z auto rotation speed" },
			{ "MainMenu.Settings.TranslationSpeed", "Translation speed" },
			{ "MainMenu.Settings.TranslationFactorSpeed", "Translation factor speed" },
			{ "MainMenu.Settings.RotationSpeed", "Rotation speed" },
			{ "MainMenu.Settings.InverseYAxis", "Inverse camera Y axis" },

			{ "MainMenu.OpenAPI", "Open from API" },
			{ "MainMenu.Undo", "Undo" },
			{ "MainMenu.Redo", "Redo" },
			// Console.
			{ "Console.Console", "Console" },

			// Scene.
			{ "Scene.Scene", "Scene" },
			{ "Scene.Performance", "Performance" },

			// Camera editor.
			{ "CameraEditor.CameraEditor", "Camera editor" },

			// Inspector.
			{ "Inspector.Inspector", "Inspector" },
			{ "Inspector.Transform", "Transform" },
			{ "Inspector.Transform.Position", "Position" },
			{ "Inspector.Transform.Rotation", "Rotation" },
			{ "Inspector.Transform.Scale", "Scale" },

			// Views.
			{ "View.Data", "Data" },
			{ "View.Play", "Play" },
			{ "View.Stop", "Stop" },
			{ "View.Path", "Path" },
			{ "View.Replace", "Replace" },
			{ "View.Delete", "Delete" },
			{ "View.Duration", "Duration" },
			{ "View.Viewpoint", "Viewpoint" },
			{ "View.Viewpoint.Add", "Add viewpoint" },
			{ "View.Duration", "Duration" },
			{ "View.DurationMode", "Speed mode" },
			{ "View.InterpolationMode", "Smooth mode" },

			// Enums.
			{ "Enum.Theme.Light", "Light" },
			{ "Enum.Theme.Dark", "Dark" },
			{ "Enum.Theme.Classic", "Classic" },

			{ "Enum.SymbolDisplay.Short", "Short" },
			{ "Enum.SymbolDisplay.Long", "Long" },

			{ "Enum.Representation.BallsAndSticks", "Balls and sticks" },
			{ "Enum.Representation.VanDerWaals", "Van der Waals" },
			{ "Enum.Representation.Sticks", "Sticks" },

			{ "Enum.ColorMode.Atom", "Atom" },
			{ "Enum.ColorMode.Residue", "Residue" },
			{ "Enum.ColorMode.Chain", "Chain" },

			{ "Enum.Shading.Lambert", "Lambert" },
			{ "Enum.Shading.BlinnPhong", "Blinn Phong" },
			{ "Enum.Shading.Toon", "Toon" },

			{ "Enum.DurationMode.Viewpoint", "Viewpoint duration" },
			{ "Enum.DurationMode.Path", "Path duration" },
			{ "Enum.DurationMode.ConstantSpeed", "Constant speed" },

			{ "Enum.InterpolationMode.Linear", "Linear" },
			{ "Enum.InterpolationMode.CatmullRom", "Catmull-rom" },
		};

	} // namespace Localization
} // namespace VTX
#endif
