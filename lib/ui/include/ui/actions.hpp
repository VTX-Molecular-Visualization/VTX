#ifndef __VTX_UI_DESCRIPTORS__
#define __VTX_UI_DESCRIPTORS__

#include <app/action/animation.hpp>
#include <app/action/application.hpp>
#include <app/application/scene.hpp>
#include <app/application/system/action_manager.hpp>
#include <functional>
#include <string>

namespace VTX::UI
{

	struct DescAction
	{
		std::string				   name;
		std::optional<std::string> group = std::nullopt;
		std::optional<std::string> tip	 = std::nullopt;
		std::optional<std::string> icon	 = std::nullopt;
		std::optional<std::string> shortcut;

		using TriggerFunction				   = std::function<void()>;
		std::optional<TriggerFunction> trigger = std::nullopt;
	};

	namespace Action
	{

		// System.
		namespace System
		{
			struct New : public DescAction
			{
				New()
				{
					name	 = "New";
					tip		 = "Create a new project";
					icon	 = ":/sprite/file/new.png";
					shortcut = "Ctrl+N";
					trigger	 = []() { App::VTX_ACTION().execute<App::Action::Application::NewScene>(); };
				}
			};

			struct Download : public DescAction
			{
				Download()
				{
					name	 = "Download";
					tip		 = "Download structure from PDB id";
					icon	 = ":/sprite/file/download.png";
					shortcut = "Ctrl+D";
				}
			};

			struct Open : public DescAction
			{
				Open()
				{
					name	 = "Open";
					tip		 = "Open a project or a molecular file";
					icon	 = ":/sprite/file/open.png";
					shortcut = "Ctrl+O";
				}
			};

			struct OpenRecent : public DescAction
			{
				OpenRecent()
				{
					name = "Open recent";
					tip	 = "Open a recent document";
					icon = ":/sprite/file/open_recent.png";
				}
			};

			struct Save : public DescAction
			{
				Save()
				{
					name	 = "Save";
					tip		 = "Save project";
					icon	 = ":/sprite/file/save.png";
					shortcut = "Ctrl+S";
				}
			};

			struct SaveAs : public DescAction
			{
				SaveAs()
				{
					name	 = "Save as...";
					tip		 = "Copy project in a new save";
					icon	 = ":/sprite/file/save_as.png";
					shortcut = "Ctrl+Shift+S";
				}
			};

			struct Import : public DescAction
			{
				Import() { name = "Import"; }
			};

			struct Export : public DescAction
			{
				Export() { name = "Export"; }
			};

			struct Quit : public DescAction
			{
				Quit()
				{
					name	 = "Quit";
					tip		 = "Exit software";
					trigger	 = []() { App::VTX_ACTION().execute<App::Action::Application::Quit>(); };
					shortcut = "Esc";
				}
			};
		} // namespace System

		// Camera.
		namespace Camera
		{
			struct Orthographic : public DescAction
			{
				Orthographic()
				{
					name	 = "Orthographic";
					group	 = "CameraProjection";
					tip		 = "Change projection mode (perspective/orthographic)";
					icon	 = ":/sprite/camera/orthographic.png";
					shortcut = "Alt+O";
				}
			};

			struct Perspective : public DescAction
			{
				Perspective()
				{
					name	 = "Perspective";
					group	 = "CameraProjection";
					tip		 = "Change projection mode (perspective/orthographic)";
					icon	 = ":/sprite/camera/perspective.png";
					shortcut = "Alt+P";
				}
			};

			struct ToggleProjection : public DescAction
			{
			};

			struct Trackball : public DescAction
			{
				Trackball()
				{
					name	 = "Trackball";
					group	 = "CameraController";
					tip		 = "Use Trackball controller";
					icon	 = ":/sprite/camera/trackball.png";
					shortcut = "Alt+T";
				}
			};

			struct Freefly : public DescAction
			{
				Freefly()
				{
					name	 = "Freefly";
					group	 = "CameraController";
					tip		 = "Use Freefly controller";
					icon	 = ":/sprite/camera/freefly.png";
					shortcut = "Alt+F";
				}
			};

			struct ToggleController : public DescAction
			{
			};

			struct Orient : public DescAction
			{
				Orient()
				{
					name = "Orient";
					tip	 = "Orient camera on selection";
					icon = ":/sprite/camera/orient.png";
					trigger
						= []() { App::VTX_ACTION().execute<App::Action::Animation::Orient>( App::SCENE().getAABB() ); };
				}
			};

			struct Reset : public DescAction
			{
				Reset()
				{
					name	= "Reset";
					tip		= "Reset camera";
					icon	= ":/sprite/camera/reset.png";
					trigger = []() { App::VTX_ACTION().execute<App::Action::Animation::ResetCamera>(); };
				}
			};
		} // namespace Camera
		namespace Snapshot
		{
			struct Snapshot : public DescAction
			{
				Snapshot()
				{
					name	 = "Snapshot";
					tip		 = "Save current image";
					icon	 = ":/sprite/snapshot/snapshot.png";
					shortcut = "F2";
				}
			};

			struct Export : public DescAction
			{
				Export()
				{
					name	 = "Export";
					tip		 = "Open dialog to export image";
					icon	 = ":/sprite/snapshot/export.png";
					shortcut = "F3";
				}
			};
		} // namespace Snapshot
		namespace Theme
		{
			struct System : public DescAction
			{
				System()
				{
					name  = "System";
					group = "Theme";
					tip	  = "Use system theme";
				}
			};

			struct Light : public DescAction
			{
				Light()
				{
					name  = "Light";
					group = "Theme";
					tip	  = "Use light theme";
				}
			};

			struct Dark : public DescAction
			{
				Dark()
				{
					name  = "Dark";
					group = "Theme";
					tip	  = "Use dark theme";
				}
			};

			struct ResetLayout : public DescAction
			{
				ResetLayout() { name = "Reset layout"; }
			};
		} // namespace Theme
		namespace Help
		{
			struct Documentation : public DescAction
			{
				Documentation() { name = "Documentation"; }
			};

			struct Report : public DescAction
			{
				Report() { name = "Report a bug"; }
			};

			struct CheckUpdates : public DescAction
			{
				CheckUpdates() { name = "Check for updates"; }
			};

			struct About : public DescAction
			{
				About() { name = "About"; }
			};

		} // namespace Help
	} // namespace Action
} // namespace VTX::UI

#endif
