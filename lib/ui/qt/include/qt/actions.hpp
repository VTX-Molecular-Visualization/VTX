#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include <QAction>
#include <QActionGroup>
#include <app/action/animation.hpp>
#include <app/action/application.hpp>
#include <app/application/scene.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/ui/concepts.hpp>
#include <app/vtx_app.hpp>
#include <util/hashed_collection.hpp>
#include <util/hashing.hpp>

namespace VTX::UI::QT
{
	template<typename A>
	concept ConceptAction = std::is_base_of_v<App::UI::DescAction, A>;

	namespace Action
	{
		struct Factory
		{
		  public:
			inline static QAction * const get( const App::UI::DescAction & p_action )
			{
				return _getOrCreate( Hash( &p_action ), p_action );
			}

			template<ConceptAction A>
			inline static QAction * get()
			{
				return _getOrCreate( Hash( typeid( A ).name() ), A() );
			}

		  private:
			inline static Util::HashedCollection<QAction *>		 ACTIONS;
			inline static Util::HashedCollection<QActionGroup *> ACTION_GROUPS;

			static QAction * const _getOrCreate(
				const Hash & p_hash,
				const App::UI::DescAction & p_action
			);
		};

		// System.
		namespace System
		{
			struct New : public App::UI::DescAction
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

			struct Download : public App::UI::DescAction
			{
				Download()
				{
					name	 = "Download";
					tip		 = "Download structure from PDB id";
					icon	 = ":/sprite/file/download.png";
					shortcut = "Ctrl+D";
				}
			};

			struct Open : public App::UI::DescAction
			{
				Open()
				{
					name	 = "Open";
					tip		 = "Open a project or a molecular file";
					icon	 = ":/sprite/file/open.png";
					shortcut = "Ctrl+O";
				}
			};

			struct OpenRecent : public App::UI::DescAction
			{
				OpenRecent()
				{
					name = "Open recent";
					tip	 = "Open a recent document";
					icon = ":/sprite/file/open_recent.png";
				}
			};

			struct Save : public App::UI::DescAction
			{
				Save()
				{
					name	 = "Save";
					tip		 = "Save project";
					icon	 = ":/sprite/file/save.png";
					shortcut = "Ctrl+S";
				}
			};

			struct SaveAs : public App::UI::DescAction
			{
				SaveAs()
				{
					name	 = "Save as...";
					tip		 = "Copy project in a new save";
					icon	 = ":/sprite/file/save_as.png";
					shortcut = "Ctrl+Shift+S";
				}
			};

			struct Import : public App::UI::DescAction
			{
				Import() { name = "Import"; }
			};

			struct Export : public App::UI::DescAction
			{
				Export() { name = "Export"; }
			};

			struct Quit : public App::UI::DescAction
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
			struct Orthographic : public App::UI::DescAction
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

			struct Perspective : public App::UI::DescAction
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

			struct ToggleProjection : public App::UI::DescAction
			{
			};

			struct Trackball : public App::UI::DescAction
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

			struct Freefly : public App::UI::DescAction
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

			struct ToggleController : public App::UI::DescAction
			{
			};

			struct Orient : public App::UI::DescAction
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

			struct Reset : public App::UI::DescAction
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
			struct Snapshot : public App::UI::DescAction
			{
				Snapshot()
				{
					name	 = "Snapshot";
					tip		 = "Save current image";
					icon	 = ":/sprite/snapshot/snapshot.png";
					shortcut = "F2";
				}
			};

			struct Export : public App::UI::DescAction
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
			struct System : public App::UI::DescAction
			{
				System()
				{
					name  = "System";
					group = "Theme";
					tip	  = "Use system theme";
				}
			};

			struct Light : public App::UI::DescAction
			{
				Light()
				{
					name  = "Light";
					group = "Theme";
					tip	  = "Use light theme";
				}
			};

			struct Dark : public App::UI::DescAction
			{
				Dark()
				{
					name  = "Dark";
					group = "Theme";
					tip	  = "Use dark theme";
				}
			};

			struct ResetLayout : public App::UI::DescAction
			{
				ResetLayout() { name = "Reset layout"; }
			};
		} // namespace Theme
		namespace Help
		{
			struct Documentation : public App::UI::DescAction
			{
				Documentation() { name = "Documentation"; }
			};

			struct Report : public App::UI::DescAction
			{
				Report() { name = "Report a bug"; }
			};

			struct CheckUpdates : public App::UI::DescAction
			{
				CheckUpdates() { name = "Check for updates"; }
			};

			struct About : public App::UI::DescAction
			{
				About() { name = "About"; }
			};

		} // namespace Help
		namespace Option
		{
			struct ShowToolBarText : public App::UI::DescAction
			{
				ShowToolBarText()
				{
					name = "Show toolbar text";
					tip	 = "Show/hide text beside tool buttons";
				}
			};
		} // namespace Option

	} // namespace Action

} // namespace VTX::UI::QT

#endif
