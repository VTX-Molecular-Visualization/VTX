#include "ui/qt/actions.hpp"
#include "app/action/controller.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/dialog/download.hpp"
#include "ui/qt/dialog/export_image.hpp"
#include "ui/qt/dialog/open.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/style.hpp"
#include <app/action/io.hpp>
#include <app/action/scene.hpp>

namespace VTX::UI::QT::Action
{

	// System.
	namespace System
	{

		New::New()
		{
			name	 = "New";
			tip		 = "Create a new project";
			icon	 = "sprite/file/new.png";
			shortcut = "Ctrl+N";
			trigger	 = []() { App::ACTION().execute<App::Action::Scene::Clear>(); };
		}

		Download::Download()
		{
			name	 = "Download";
			tip		 = "Download structure from PDB id";
			icon	 = "sprite/file/download.png";
			shortcut = "Ctrl+D";
			trigger	 = []()
			{
				Dialog::Download dialog;
				dialog.exec();
			};
		}

		Open::Open()
		{
			name	 = "Open";
			tip		 = "Open a project or a molecular file";
			icon	 = "sprite/file/open.png";
			shortcut = "Ctrl+O";
			trigger	 = []()
			{
				Dialog::Open dialog;
				dialog.exec();
			};
		}

		OpenRecent::OpenRecent()
		{
			name = "Open recent";
			tip	 = "Open a recent document";
			icon = "sprite/file/open_recent.png";
		}

		Delete::Delete()
		{
			name = "Delete";
			tip	 = "Delete system";
			// icon = "sprite/file/open_recent.png";
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
			trigger	 = []() { QCoreApplication::quit(); };
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
			tip		 = "Change camera projection mode";
			icon	 = "sprite/camera/orthographic.png";
			shortcut = "Alt+O";
			// trigger	 = []() {
			// App::ACTION().execute<App::Action::Controller::SetCameraController<App::Pass::Controller::Trackball>>();
			// };
		}

		void Orthographic::connect() const
		{
			/*
			using namespace App::Component;

			QAction * const	 qAction = Factory::get<Orthographic>();
			Render::Camera & camera	 = App::SCENE().getCamera();

			if ( camera.getProjection() == Render::Camera::PROJECTION::ORTHOGRAPHIC )
			{
				qAction->setChecked( true );
			}

			camera.onProjectionChange += [ qAction ]( const Render::Camera::PROJECTION p_projection )
			{
				if ( p_projection == Render::Camera::PROJECTION::ORTHOGRAPHIC )
				{
					qAction->setChecked( true );
				}
			};
			*/
		}

		Perspective::Perspective()
		{
			name	 = "Perspective";
			group	 = "CameraProjection";
			tip		 = "Change camera projection mode";
			icon	 = "sprite/camera/perspective.png";
			shortcut = "Alt+P";
			// trigger	 = []() { App::ACTION().execute<App::Action::Camera::ToggleCameraProjection>(); };
		}

		void Perspective::connect() const
		{
			/*
			using namespace App::Component;

			QAction * const	 qAction = Factory::get<Perspective>();
			Render::Camera & camera	 = App::SCENE().getCamera();

			if ( camera.getProjection() == Render::Camera::PROJECTION::PERSPECTIVE )
			{
				qAction->setChecked( true );
			}

			camera.onProjectionChange += [ qAction ]( const Render::Camera::PROJECTION p_projection )
			{
				if ( p_projection == Render::Camera::PROJECTION::PERSPECTIVE )
				{
					qAction->setChecked( true );
				}
			};
			*/
		}

		Trackball::Trackball()
		{
			name	 = "Trackball";
			group	 = "CameraController";
			tip		 = "Use Trackball controller";
			icon	 = "sprite/camera/trackball.png";
			shortcut = "Alt+T";
			trigger	 = []()
			{
				App::ACTION().execute<App::Action::Controller::SetCameraController<App::Pass::Controller::Trackball>>();
			};
		}

		void Trackball::connect() const
		{
			QAction * const qAction = Application::getAction<Trackball>();
			App::HUB().connect<App::Events::CameraControllerChange<App::Pass::Controller::Trackball>>(
				[ qAction ]() { qAction->setChecked( true ); }
			);
		}

		Freefly::Freefly()
		{
			name	 = "Freefly";
			group	 = "CameraController";
			tip		 = "Use Freefly controller";
			icon	 = "sprite/camera/freefly.png";
			shortcut = "Alt+F";
			trigger	 = []()
			{ App::ACTION().execute<App::Action::Controller::SetCameraController<App::Pass::Controller::Freefly>>(); };
		}

		void Freefly::connect() const
		{
			QAction * const qAction = Application::getAction<Freefly>();
			App::HUB().connect<App::Events::CameraControllerChange<App::Pass::Controller::Freefly>>(
				[ qAction ]() { qAction->setChecked( true ); }
			);
		}

		Orient::Orient()
		{
			name = "Orient";
			tip	 = "Orient camera on selection";
			icon = "sprite/camera/orient.png";
			// trigger = []() { App::ACTION().execute<App::Action::Camera::Orient>(); };
		}

		Reset::Reset()
		{
			name = "Reset";
			tip	 = "Reset camera";
			icon = "sprite/camera/reset.png";
			// trigger = []() { App::ACTION().execute<App::Action::Camera::Reset>(); };
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
			trigger	 = []()
			{
				App::Action::IO::Snapshot action;
				App::ACTION().execute( action );
			};
		}

		Export::Export()
		{
			name	 = "Export";
			tip		 = "Open dialog to export image";
			icon	 = "sprite/snapshot/export.png";
			shortcut = "F3";
			trigger	 = []()
			{
				Dialog::ExportImage dialog;
				dialog.exec();
			};
		}

	} // namespace Snapshot
	namespace Selection
	{

		Lock::Lock()
		{
			name = "Lock";
			tip	 = "Lock the current selection";
			// icon = static_cast<int>( QStyle::StandardPixmap::SP_TitleBarShadeButton );
		}

		Save::Save()
		{
			name = "Save";
			tip	 = "Save the current selection";
			// icon = static_cast<int>( QStyle::StandardPixmap::SP_DialogSaveButton );
		}

		Clear::Clear()
		{
			name = "Clear";
			tip	 = "Clear selection";
			// icon = static_cast<int>( QStyle::StandardPixmap::SP_TrashIcon );
			trigger = []() { SELECTION().clear(); };
		}

		SetGranularitySystem::SetGranularitySystem()
		{
			name  = "System";
			group = "SelectionGranularity";
			tip	  = "Change selection granularity to system";
			// icon = "sprite/selection/granularity.png";
			//  shortcut = "G";
			//   trigger	 = []() {
			//   App::ACTION().execute<App::Action::Application::ChangeSelectionGranularity>();
			//   };
		}

		SetGranularityChain::SetGranularityChain()
		{
			name  = "Chain";
			group = "SelectionGranularity";
			tip	  = "Change selection granularity to chain";
			// icon = "sprite/selection/granularity.png";
			//  shortcut = "G";
			//   trigger	 = []() {
			//   App::ACTION().execute<App::Action::Application::ChangeSelectionGranularity>();
			//   };
		}

		SetGranularityResidue::SetGranularityResidue()
		{
			name  = "Residue";
			group = "SelectionGranularity";
			tip	  = "Change selection granularity to residue";
			// icon = "sprite/selection/granularity.png";
			//  shortcut = "G";
			//   trigger	 = []() {
			//   App::ACTION().execute<App::Action::Application::ChangeSelectionGranularity>();
			//   };
		}

		SetGranularityAtom::SetGranularityAtom()
		{
			name  = "Atom";
			group = "SelectionGranularity";
			tip	  = "Change selection granularity to atom";
			// icon = "sprite/selection/granularity.png";
			//  shortcut = "G";
			//   trigger	 = []() {
			//   App::ACTION().execute<App::Action::Application::ChangeSelectionGranularity>();
			//   };
		}
	} // namespace Selection
	namespace Theme
	{

		System::System()
		{
			name	= "System";
			group	= "Theme";
			tip		= "Use system theme";
			trigger = []() { STYLE().setTheme( Style::E_THEME::SYSTEM ); };
		}

		Light::Light()
		{
			name	= "Light";
			group	= "Theme";
			tip		= "Use light theme";
			trigger = []() { STYLE().setTheme( Style::E_THEME::LIGHT ); };
		}

		Dark::Dark()
		{
			name	= "Dark";
			group	= "Theme";
			tip		= "Use dark theme";
			trigger = []() { STYLE().setTheme( Style::E_THEME::DARK ); };
		}

		ResetLayout::ResetLayout() { name = "Reset layout"; }

	} // namespace Theme
	namespace Option
	{
		namespace Cache
		{
			Open::Open()
			{
				name = "Open";
				tip	 = "Open the cache folder in explorer";
			}

			Clear::Clear()
			{
				name = "Clear";
				tip	 = "Delete all cached files";
			}

			Refresh::Refresh()
			{
				name = "Refresh";
				tip	 = "Refresh cache size";
			}
		} // namespace Cache
	} // namespace Option
	namespace Help
	{

		Documentation::Documentation() { name = "Documentation"; }

		Report::Report() { name = "Report a bug"; }

		CheckUpdates::CheckUpdates() { name = "Check for updates"; }

		About::About() { name = "About"; }

	} // namespace Help

} // namespace VTX::UI::QT::Action
