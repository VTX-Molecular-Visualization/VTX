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

			{ "Load", "Load" },
			{ "Save", "Save" },

			// TODO: remove extended keys, juste translate.

			// Main menu.
			{ "MainMenu.Menu", "Menu" },
			{ "MainMenu.Menu.New", "New" },
			{ "MainMenu.Menu.Open", "Open" },

			{ "MainMenu.Menu.Open.ChooseFile", "Choose file" },
			{ "MainMenu.Menu.Quit", "Quit" },
			{ "MainMenu.Import", "Import" },
			{ "MainMenu.Import.MMTF", "MMTF" },
			{ "MainMenu.Display", "Display" },
			{ "MainMenu.Display.Render", "Render" },
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
			{ "MainMenu.Export.RenderCPU", "Render CPU" },
			{ "MainMenu.Export.RenderOptix", "Render Optix" },
			{ "MainMenu.Export.Path", "Path" },
			{ "MainMenu.Export.Video", "Video" },
			{ "MainMenu.Export.VideoRT", "Video RT" },
			{ "MainMenu.Settings", "Options" },
			{ "MainMenu.Settings.Theme", "Theme" },
			{ "MainMenu.Settings.SymbolDisplay", "Symbol display" },
			{ "MainMenu.Settings.Rendering", "Rendering" },
			{ "MainMenu.Settings.BackgroundColor", "Background color" },
			{ "MainMenu.Settings.Representation", "Representation" },
			{ "MainMenu.Settings.AtomsRadius", "Atoms radius" },
			{ "MainMenu.Settings.BondsRadius", "Bonds radius" },
			{ "MainMenu.Settings.ColorMode", "Color mode" },
			{ "MainMenu.Settings.Shading", "Shading" },
			{ "MainMenu.Settings.CamNear", "Near clip" },
			{ "MainMenu.Settings.CamFar", "Far clip" },
			{ "MainMenu.Settings.CamPerspective", "Perspective projection" },
			{ "MainMenu.Settings.CamFov", "FOV" },
			{ "MainMenu.Settings.VSync", "VSync" },
			{ "MainMenu.Settings.SSAO", "SSAO" },
			{ "MainMenu.Settings.AOIntensity", "AO intensity" },
			{ "MainMenu.Settings.AOBlurSize", "AO blur size" },
			{ "MainMenu.Settings.Outline", "Outline" },
			{ "MainMenu.Settings.AA", "Anti-aliasing" },
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

			{ "MainMenu.Camera", "Camera" },

			// Render.
			{ "Render.Render", "Render" },

			// Console.
			{ "Console.Console", "Console" },

			// Scene.
			{ "Scene.Scene", "Scene" },
			{ "Scene.Performance", "Performance" },

			// Camera editor.
			{ "CameraEditor.CameraEditor", "Camera editor" },

			// Inspector.
			{ "Inspector.Inspector", "Inspector" },

			// Views.
			{ "View.Data", "Data" },
			{ "View.Chain", "Chain" },
			{ "View.Chains%Count", "Chains: %d" },
			{ "View.Residue", "Residue" },
			{ "View.Residues%Count", "Residues: %d" },
			{ "View.Atom", "Atom" },
			{ "View.Atoms%Count", "Atoms: %d" },
			{ "View.Bond", "Bond" },
			{ "View.Bonds%Count", "Bonds: %d" },
			{ "View.Options", "Options" },
			{ "View.Play", "Play" },
			{ "View.Stop", "Stop" },
			{ "View.Loop", "Loop" },
			{ "View.Path", "Path" },
			{ "View.Frame", "Frame" },
			{ "View.FPS", "FPS" },
			{ "View.Add", "Add" },
			{ "View.Replace", "Replace" },
			{ "View.Delete", "Delete" },
			{ "View.Duration", "Duration" },
			{ "View.Color", "Color" },
			{ "View.Viewpoint", "Viewpoint" },
			{ "View.Viewpoint.Add", "Add viewpoint" },
			{ "View.Duration", "Duration" },
			{ "View.DurationMode", "Speed mode" },
			{ "View.InterpolationMode", "Smooth mode" },
			{ "View.Transform", "Transform" },
			{ "View.Transform.Position", "Position" },
			{ "View.Transform.Rotation", "Rotation" },
			{ "View.Transform.Scale", "Scale" },
			{ "View.Molecule.Dynamic", "Dynamic" },
			{ "View.Molecule.Solvent", "Solvent" },
			{ "View.Molecule.Ion", "Ion" },
			{ "View.Actions", "Actions" },
			{ "View.Visible", "Visible" },
			{ "View.SecondaryStructure", "Secondary Structure" },
			{ "View.SecondaryStructure%State", "Secondary Structure: %s" },
			{ "View.Loaded", "Loaded" },
			{ "View.Computed", "Computed" },
			{ "View.Compute", "Compute" },
			{ "View.Show", "Show" },
			{ "View.Hide", "Hide" },
			{ "View.Solo", "Solo" },
			{ "View.All", "All" },
			{ "View.Representation", "Representation" },

			// Modals.
			{ "Modal.Loading", "Loading" },

			// Enums.
			{ "Enum.Theme.Light", "Light" },
			{ "Enum.Theme.Dark", "Dark" },
			{ "Enum.Theme.Classic", "Classic" },

			{ "Enum.SymbolDisplay.Short", "Short" },
			{ "Enum.SymbolDisplay.Long", "Long" },

			{ "Enum.Representation.BallsAndSticks", "Balls and sticks" },
			{ "Enum.Representation.VanDerWaals", "Van der Waals" },
			{ "Enum.Representation.Sticks", "Sticks" },
			{ "Enum.Representation.SAS", "SAS" },
			{ "Enum.Representation.Cartoon", "Cartoon" },

			{ "Enum.ColorMode.Atom", "Atom" },
			{ "Enum.ColorMode.Residue", "Residue" },
			{ "Enum.ColorMode.Chain", "Chain" },
			{ "Enum.ColorMode.Protein", "Protein" },

			{ "Enum.Shading.Diffuse", "Diffuse" },
			{ "Enum.Shading.Glossy", "Glossy" },
			{ "Enum.Shading.Toon", "Toon" },
			{ "Enum.Shading.FlatColor", "Flat Color" },

			{ "Enum.DurationMode.Viewpoint", "Viewpoint duration" },
			{ "Enum.DurationMode.Path", "Path duration" },
			{ "Enum.DurationMode.ConstantSpeed", "Constant speed" },

			{ "Enum.InterpolationMode.Linear", "Linear" },
			{ "Enum.InterpolationMode.CatmullRom", "Catmull-rom" },
			{ "Enum.InterpolationMode.Cubic", "Cubic" },

			{ "Enum.API.MMTF", "MMTF" },
			{ "Enum.API.PDB", "PDB" },
		};

	} // namespace Localization
} // namespace VTX
#endif
