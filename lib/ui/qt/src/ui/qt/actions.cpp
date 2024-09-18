#include "ui/qt/actions.hpp"
#include <app/action/animation.hpp>
#include <app/action/application.hpp>
#include <app/action/controller.hpp>
#include <app/application/scene.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Action
{
	QAction * const Factory::_getOrCreate( const Hash & p_hash, const App::UI::DescAction & p_action )
	{
		if ( not _ACTIONS.has( p_hash ) )
		{
			QAction * qAction = _ACTIONS.create<QAction>( p_hash );

			VTX_TRACE( "UI action created: {}", p_action.name );

			// Name.
			qAction->setText( p_action.name.c_str() );
			// Group.
			if ( p_action.group.has_value() )
			{
				const auto group = Util::hash( p_action.group.value() );
				if ( not _ACTION_GROUPS.has( group ) )
				{
					_ACTION_GROUPS.set( group, new QActionGroup( nullptr ) );
				}

				qAction->setCheckable( true );
				_ACTION_GROUPS.get<QActionGroup>( group )->addAction( qAction );
			}
			// Tip.
			if ( p_action.tip.has_value() )
			{
				QString tip = p_action.tip.value().c_str();

				if ( p_action.shortcut.has_value() )
				{
					tip.append( " (" + p_action.shortcut.value() + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( p_action.icon.has_value() )
			{
				qAction->setIcon( QIcon( ( ":/" + p_action.icon.value() ).c_str() ) );
			}
			// Shortcut.
			if ( p_action.shortcut.has_value() )
			{
				qAction->setShortcut( QKeySequence( p_action.shortcut.value().c_str() ) );
			}
			// Action.
			if ( p_action.trigger.has_value() )
			{
				QObject::connect( qAction, &QAction::triggered, p_action.trigger.value() );
			}
		}

		return _ACTIONS.get<QAction>( p_hash );
	}

	// System.
	namespace System
	{

		New::New()
		{
			name	 = "New";
			tip		 = "Create a new project";
			icon	 = "sprite/file/new.png";
			shortcut = "Ctrl+N";
			trigger	 = []() { App::ACTION_SYSTEM().execute<App::Action::Application::NewScene>(); };
		}

		Download::Download()
		{
			name	 = "Download";
			tip		 = "Download structure from PDB id";
			icon	 = "sprite/file/download.png";
			shortcut = "Ctrl+D";
		}

		Open::Open()
		{
			name	 = "Open";
			tip		 = "Open a project or a molecular file";
			icon	 = "sprite/file/open.png";
			shortcut = "Ctrl+O";
		}

		OpenRecent::OpenRecent()
		{
			name = "Open recent";
			tip	 = "Open a recent document";
			icon = "sprite/file/open_recent.png";
		}

		Save::Save()
		{
			name	 = "Save";
			tip		 = "Save project";
			icon	 = "sprite/file/save.png";
			shortcut = "Ctrl+S";
		}

		SaveAs::SaveAs()
		{
			name	 = "Save as...";
			tip		 = "Copy project in a new save";
			icon	 = "sprite/file/save_as.png";
			shortcut = "Ctrl+Shift+S";
		}

		Import::Import() { name = "Import"; }

		Export::Export() { name = "Export"; }

		Quit::Quit()
		{
			name	 = "Quit";
			tip		 = "Exit software";
			trigger	 = []() { App::ACTION_SYSTEM().execute<App::Action::Application::Quit>(); };
			shortcut = "Esc";
		}

	} // namespace System

	// Camera.
	namespace Camera
	{

		Orthographic::Orthographic()
		{
			name	 = "Orthographic";
			group	 = "CameraProjection";
			tip		 = "Change projection mode (perspective/orthographic)";
			icon	 = "sprite/camera/orthographic.png";
			shortcut = "Alt+O";
		}

		Perspective::Perspective()
		{
			name	 = "Perspective";
			group	 = "CameraProjection";
			tip		 = "Change projection mode (perspective/orthographic)";
			icon	 = "sprite/camera/perspective.png";
			shortcut = "Alt+P";
		}

		Trackball::Trackball()
		{
			name	 = "Trackball";
			group	 = "CameraController";
			tip		 = "Use Trackball controller";
			icon	 = "sprite/camera/trackball.png";
			shortcut = "Alt+T";
			trigger	 = []() { App::ACTION_SYSTEM().execute<App::Action::Controller::ToggleCameraController>(); };
		}

		Freefly::Freefly()
		{
			name	 = "Freefly";
			group	 = "CameraController";
			tip		 = "Use Freefly controller";
			icon	 = "sprite/camera/freefly.png";
			shortcut = "Alt+F";
			trigger	 = []() { App::ACTION_SYSTEM().execute<App::Action::Controller::ToggleCameraController>(); };
		}

		Orient::Orient()
		{
			name	= "Orient";
			tip		= "Orient camera on selection";
			icon	= "sprite/camera/orient.png";
			trigger = []() { App::ACTION_SYSTEM().execute<App::Action::Animation::Orient>( App::SCENE().getAABB() ); };
		}

		Reset::Reset()
		{
			name	= "Reset";
			tip		= "Reset camera";
			icon	= "sprite/camera/reset.png";
			trigger = []() { App::ACTION_SYSTEM().execute<App::Action::Animation::ResetCamera>(); };
		}

	} // namespace Camera
	namespace Snapshot
	{

		Snapshot::Snapshot()
		{
			name	 = "Snapshot";
			tip		 = "Save current image";
			icon	 = "sprite/snapshot/snapshot.png";
			shortcut = "F2";
		}

		Export::Export()
		{
			name	 = "Export";
			tip		 = "Open dialog to export image";
			icon	 = "sprite/snapshot/export.png";
			shortcut = "F3";
		}

	} // namespace Snapshot
	namespace Theme
	{

		System::System()
		{
			name  = "System";
			group = "Theme";
			tip	  = "Use system theme";
		}

		Light::Light()
		{
			name  = "Light";
			group = "Theme";
			tip	  = "Use light theme";
		}

		Dark::Dark()
		{
			name  = "Dark";
			group = "Theme";
			tip	  = "Use dark theme";
		}

		ResetLayout::ResetLayout() { name = "Reset layout"; }

	} // namespace Theme
	namespace Help
	{

		Documentation::Documentation() { name = "Documentation"; }

		Report::Report() { name = "Report a bug"; }

		CheckUpdates::CheckUpdates() { name = "Check for updates"; }

		About::About() { name = "About"; }

	} // namespace Help
	namespace Option
	{

		ShowToolBarText::ShowToolBarText()
		{
			name = "Show toolbar text";
			tip	 = "Show/hide text beside tool buttons";
		}

	} // namespace Option
} // namespace VTX::UI::QT::Action
