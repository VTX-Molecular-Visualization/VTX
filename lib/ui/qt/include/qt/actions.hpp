#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include "helper.hpp"
#include "resources.hpp"
#include <QAction>
#include <app/action/application.hpp>
#include <app/application/system/action_manager.hpp>
#include <ui/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename A>
	concept ConceptAction = std::is_base_of_v<UI::Action, A>;

	static std::map<std::string, QAction *> ACTIONS;

	template<ConceptAction T>
	static QAction * const ACTION()
	{
		const std::string name = typeid( T ).name();

		if ( not ACTIONS.contains( name ) )
		{
			T		  action;
			QAction * qAction = new QAction();
			ACTIONS[ name ]	  = qAction;

			VTX_DEBUG( "Action created: {}", name );

			// Name.
			qAction->setText( action.name.c_str() );
			// Tip.
			if ( action.tip.has_value() )
			{
				QString tip = action.tip.value().c_str();
				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( action.icon.has_value() )
			{
				qAction->setIcon( QIcon( action.icon.value().c_str() ) );
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
					name	= "New";
					tip		= "Create a new project";
					trigger = []() { App::VTX_ACTION().execute<App::Action::Application::NewScene>(); };
				}
			};

			struct Download : public UI::Action
			{
				Download()
				{
					name = "Download";
					tip	 = "Download structure from PDB id";
				}
			};

			struct Open : public UI::Action
			{
				Open()
				{
					name = "Open";
					tip	 = "Open a project or a molecular file";
				}
			};

			struct OpenRecent : public UI::Action
			{
				OpenRecent() { name = "Open recent"; }
			};

			struct Save : public UI::Action
			{
				Save()
				{
					name = "Save";
					tip	 = "Save project";
				}
			};

			struct SaveAs : public UI::Action
			{
				SaveAs()
				{
					name = "Save as...";
					tip	 = "Copy project in a new save";
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

			struct ChangeProjection : public UI::Action
			{
				ChangeProjection()
				{
					name = "Perspective";
					tip	 = "Change projection mode (perspective/orthographic)";
					icon = ":/sprite/camera/perspective.png";
				}
			};

			struct Trackball : public UI::Action
			{
				Trackball()
				{
					name = "Trackball";
					tip	 = "Use Trackball controller";
					icon = ":/sprite/camera/trackball.png";
				}
			};

			struct Freefly : public UI::Action
			{
				Freefly()
				{
					name = "Freefly";
					tip	 = "Use Freefly controller";
					icon = ":/sprite/camera/freefly.png";
				}
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

	} // namespace Action

} // namespace VTX::UI::QT

#endif
