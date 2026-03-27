#include "ui/qt/actions.hpp"
#include "app/action/application.hpp"
#include "app/action/controller.hpp"
#include "app/action/selection.hpp"
#include "app/setting/controller.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/dialog/download.hpp"
#include "ui/qt/dialog/export_image.hpp"
#include "ui/qt/dialog/open.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/io.hpp>
#include <app/action/scene.hpp>
#include <app/constants.hpp>

namespace VTX::UI::QT::Action
{

	// System.
	namespace System
	{

		New::New()
		{
			name	 = "New";
			tip		 = "Create a new project";
			icon	 = Style::Icons::NEW;
			shortcut = "Ctrl+N";
			trigger	 = []() { App::ACTION().execute<App::Action::Scene::Clear>(); };
		}

		Download::Download()
		{
			name	 = "Download";
			tip		 = "Download structure from PDB id";
			icon	 = Style::Icons::DOWNLOAD;
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
			icon	 = Style::Icons::OPEN;
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
			icon = Style::Icons::DELETE;
		}

		Save::Save()
		{
			name	 = "Save";
			tip		 = "Save project";
			icon	 = Style::Icons::SAVE;
			shortcut = "Ctrl+S";
		}

		SaveAs::SaveAs()
		{
			name	 = "Save as...";
			tip		 = "Copy project in a new save";
			icon	 = Style::Icons::SAVE_AS;
			shortcut = "Ctrl+Shift+S";
		}

		Import::Import() { name = "Import"; }

		Export::Export() { name = "Export"; }

		Quit::Quit()
		{
			name	 = "Quit";
			tip		 = "Exit software";
			icon	 = Style::Icons::QUIT;
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
			icon	 = Style::Icons::CAMERA_ORTHOGRAPHIC;
			shortcut = "Alt+O";
			trigger	 = []()
			{ App::ACTION().execute<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>(); };
		}

		Perspective::Perspective()
		{
			name	 = "Perspective";
			group	 = "CameraProjection";
			tip		 = "Change camera projection mode";
			icon	 = Style::Icons::CAMERA_PERSPECTIVE;
			shortcut = "Alt+P";
			trigger	 = []()
			{ App::ACTION().execute<App::Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>(); };
		}

		Trackball::Trackball()
		{
			name	 = "Trackball";
			group	 = "CameraController";
			tip		 = "Use Trackball controller";
			icon	 = Style::Icons::CONTROLLER_TRACKBALL;
			shortcut = "Alt+T";
			trigger	 = []()
			{
				App::ACTION()
					.execute<App::Action::Controller::SetCameraController<App::Setting::E_CONTROLLER::TRACKBALL>>();
			};
		}

		Freefly::Freefly()
		{
			name	 = "Freefly";
			group	 = "CameraController";
			tip		 = "Use Freefly controller";
			icon	 = Style::Icons::CONTROLLER_FREEFLY;
			shortcut = "Alt+F";
			trigger	 = []()
			{
				App::ACTION()
					.execute<App::Action::Controller::SetCameraController<App::Setting::E_CONTROLLER::FREEFLY>>();
			};
		}

		Orient::Orient()
		{
			name	= "Orient";
			tip		= "Orient camera on selection";
			icon	= Style::Icons::CAMERA_ORIENT;
			trigger = []() { App::ACTION().execute<App::Action::Camera::Orient>(); };
		}

		Reset::Reset()
		{
			name	= "Reset";
			tip		= "Reset camera";
			icon	= Style::Icons::CAMERA_RESET;
			trigger = []() { App::ACTION().execute<App::Action::Camera::Reset>(); };
		}

	} // namespace Camera
	namespace Snapshot
	{

		Snapshot::Snapshot()
		{
			name	 = "Snapshot";
			tip		 = "Save current image";
			icon	 = Style::Icons::SNAPSHOT;
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
			icon	 = Style::Icons::SNAPSHOT_EXPORT;
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
			icon = Style::Icons::LOCK;

			// TODO: trigger/update from setting.
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
			trigger = []() { SELECTION().clearSystem(); };
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

		ResetLayout::ResetLayout()
		{
			name	= "Reset layout";
			trigger = []() { MAIN_WINDOW().resetLayout(); };
		}

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

		Documentation::Documentation()
		{
			name	= "Documentation";
			tip		= "Open online full documentation";
			icon	= Style::Icons::DOCUMENTATION;
			trigger = []() { QDesktopServices::openUrl( QUrl( App::URL_DOCUMENTATION.data() ) ); };
		}

		Report::Report()
		{
			name	= "Report a bug";
			tip		= "Report a bug or ask for a feature";
			icon	= Style::Icons::BUG;
			trigger = []() { QDesktopServices::openUrl( QUrl( App::URL_REPORT.data() ) ); };
		}

		CheckUpdates::CheckUpdates()
		{
			name	= "Check for updates";
			tip		= "Check online for a new version ";
			icon	= Style::Icons::UPDATE;
			trigger = []() { App::ACTION().execute<App::Action::Application::CheckForUpdate>(); };
		}

		About::About()
		{
			name	= "About";
			icon	= Style::Icons::INFO;
			trigger = []() {};
		}

	} // namespace Help

} // namespace VTX::UI::QT::Action
