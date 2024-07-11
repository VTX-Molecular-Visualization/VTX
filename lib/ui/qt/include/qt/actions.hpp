#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include "helper.hpp"
#include "resources.hpp"
#include <QAction>
#include <QActionGroup>
#include <app/action/application.hpp>
#include <app/application/system/action_manager.hpp>
#include <ui/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename A>
	concept ConceptAction = std::is_base_of_v<UI::Action, A>;

	extern std::map<std::string, QAction *>		 ACTIONS;
	extern std::map<std::string, QActionGroup *> ACTION_GROUPS;

	template<ConceptAction T>
	static QAction * const ACTION()
	{
		const std::string name = typeid( T ).name();

		if ( not ACTIONS.contains( name ) )
		{
			T		  action;
			QAction * qAction = new QAction();
			ACTIONS[ name ]	  = qAction;

			VTX_TRACE( "Action created: {}", name );

			// Name.
			qAction->setText( action.name.c_str() );
			// Group.
			if ( action.group.has_value() )
			{
				std::string group = action.group.value();
				if ( not ACTION_GROUPS.contains( group ) )
				{
					ACTION_GROUPS[ group ] = new QActionGroup( nullptr );
				}

				qAction->setCheckable( true );
				ACTION_GROUPS[ group ]->addAction( qAction );
			}
			// Tip.
			if ( action.tip.has_value() )
			{
				QString tip = action.tip.value().c_str();

				if ( action.shortcut.has_value() )
				{
					tip.append( " (" + action.shortcut.value() + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( action.icon.has_value() )
			{
				qAction->setIcon( QIcon( action.icon.value().c_str() ) );
			}
			// Shortcut.
			if ( action.shortcut.has_value() )
			{
				qAction->setShortcut( QKeySequence( action.shortcut.value().c_str() ) );
			}
			// Action.
			if ( action.trigger.has_value() )
			{
				QObject::connect( qAction, &QAction::triggered, action.trigger.value() );
			}
			// TODO: shortcut.
		}

		return ACTIONS[ typeid( T ).name() ];
	}

	namespace Action
	{
		// System.
		namespace System
		{
			struct New : public UI::Action
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

			struct Download : public UI::Action
			{
				Download()
				{
					name	 = "Download";
					tip		 = "Download structure from PDB id";
					icon	 = ":/sprite/file/download.png";
					shortcut = "Ctrl+D";
				}
			};

			struct Open : public UI::Action
			{
				Open()
				{
					name	 = "Open";
					tip		 = "Open a project or a molecular file";
					icon	 = ":/sprite/file/open.png";
					shortcut = "Ctrl+O";
				}
			};

			struct OpenRecent : public UI::Action
			{
				OpenRecent()
				{
					name = "Open recent";
					tip	 = "Open a recent document";
					icon = ":/sprite/file/open_recent.png";
				}
			};

			struct Save : public UI::Action
			{
				Save()
				{
					name	 = "Save";
					tip		 = "Save project";
					icon	 = ":/sprite/file/save.png";
					shortcut = "Ctrl+S";
				}
			};

			struct SaveAs : public UI::Action
			{
				SaveAs()
				{
					name	 = "Save as...";
					tip		 = "Copy project in a new save";
					icon	 = ":/sprite/file/save_as.png";
					shortcut = "Ctrl+Shift+S";
				}
			};

			struct Import : public UI::Action
			{
				Import() { name = "Import"; }
			};

			struct Export : public UI::Action
			{
				Export() { name = "Export"; }
			};

			struct Quit : public UI::Action
			{
				Quit()
				{
					name	= "Quit";
					tip		= "Exit software";
					trigger = []() { App::VTX_ACTION().execute<App::Action::Application::Quit>(); };
				}
			};
		} // namespace System

		// Camera.
		namespace Camera
		{
			struct Orthographic : public UI::Action
			{
				Orthographic()
				{
					name	 = "Orthographic";
					group	 = "CameraProjection";
					tip		 = "Change projection mode (perspective/orthographic)";
					icon	 = ":/sprite/camera/orthographic.png";
					shortcut = "Ctrl+Shift+O";
				}
			};

			struct Perspective : public UI::Action
			{
				Perspective()
				{
					name	 = "Perspective";
					group	 = "CameraProjection";
					tip		 = "Change projection mode (perspective/orthographic)";
					icon	 = ":/sprite/camera/perspective.png";
					shortcut = "Ctrl+Shift+P";
				}
			};

			struct ToggleProjection : public UI::Action
			{
			};

			struct Trackball : public UI::Action
			{
				Trackball()
				{
					name	 = "Trackball";
					group	 = "CameraController";
					tip		 = "Use Trackball controller";
					icon	 = ":/sprite/camera/trackball.png";
					shortcut = "Ctrl+Shift+T";
				}
			};

			struct Freefly : public UI::Action
			{
				Freefly()
				{
					name	 = "Freefly";
					group	 = "CameraController";
					tip		 = "Use Freefly controller";
					icon	 = ":/sprite/camera/freefly.png";
					shortcut = "Ctrl+Shift+F";
				}
			};

			struct ToggleController : public UI::Action
			{
			};

			struct Orient : public UI::Action
			{
				Orient()
				{
					name = "Orient";
					tip	 = "Orient camera on selection";
					icon = ":/sprite/camera/orient.png";
				}
			};

			struct Reset : public UI::Action
			{
				Reset()
				{
					name = "Reset";
					tip	 = "Reset camera";
					icon = ":/sprite/camera/reset.png";
				}
			};
		} // namespace Camera
		namespace Snapshot
		{
			struct Snapshot : public UI::Action
			{
				Snapshot()
				{
					name	 = "Snapshot";
					tip		 = "Save current image";
					icon	 = ":/sprite/snapshot/snapshot.png";
					shortcut = "F2";
				}
			};

			struct Export : public UI::Action
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
			struct System : public UI::Action
			{
				System()
				{
					name  = "System";
					group = "Theme";
					tip	  = "Use system theme";
				}
			};

			struct Light : public UI::Action
			{
				Light()
				{
					name  = "Light";
					group = "Theme";
					tip	  = "Use light theme";
				}
			};

			struct Dark : public UI::Action
			{
				Dark()
				{
					name  = "Dark";
					group = "Theme";
					tip	  = "Use dark theme";
				}
			};

			struct ResetLayout : public UI::Action
			{
				ResetLayout() { name = "Reset layout"; }
			};
		} // namespace Theme
		namespace Help
		{
			struct Documentation : public UI::Action
			{
				Documentation() { name = "Documentation"; }
			};

			struct Report : public UI::Action
			{
				Report() { name = "Report a bug"; }
			};

			struct CheckUpdates : public UI::Action
			{
				CheckUpdates() { name = "Check for updates"; }
			};

			struct About : public UI::Action
			{
				About() { name = "About"; }
			};

		} // namespace Help

	} // namespace Action

} // namespace VTX::UI::QT

#endif
