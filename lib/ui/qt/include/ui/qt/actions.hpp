#ifndef __VTX_UI_QT_ACTIONS__
#define __VTX_UI_QT_ACTIONS__

#include "ui/qt/style/icons.hpp"
#include <app/ui/concepts.hpp>
#include <string_view>

namespace VTX::UI::QT
{
	class ActionRegistry;

	namespace Action
	{
		void registerActions( ActionRegistry & );

		namespace System
		{
			inline constexpr std::string_view NEW		  = "system.new";
			inline constexpr std::string_view DOWNLOAD	  = "system.download";
			inline constexpr std::string_view OPEN		  = "system.open";
			inline constexpr std::string_view OPEN_RECENT = "system.openRecent";
			inline constexpr std::string_view SAVE		  = "system.save";
			inline constexpr std::string_view SAVE_AS	  = "system.saveAs";
			inline constexpr std::string_view IMPORT	  = "system.import";
			inline constexpr std::string_view EXPORT	  = "system.export";
			inline constexpr std::string_view QUIT		  = "system.quit";

			inline App::UI::DescAction newAction()
			{
				App::UI::DescAction action;
				action.key		= NEW;
				action.name		= "New";
				action.tip		= "Create a new project";
				action.icon		= Style::Icons::NEW;
				action.shortcut = "Ctrl+N";
				return action;
			}

			inline App::UI::DescAction downloadAction()
			{
				App::UI::DescAction action;
				action.key		= DOWNLOAD;
				action.name		= "Download";
				action.tip		= "Download structure from PDB id";
				action.icon		= Style::Icons::DOWNLOAD;
				action.shortcut = "Ctrl+D";
				return action;
			}

			inline App::UI::DescAction openAction()
			{
				App::UI::DescAction action;
				action.key		= OPEN;
				action.name		= "Open";
				action.tip		= "Open a project or a molecular file";
				action.icon		= Style::Icons::OPEN;
				action.shortcut = "Ctrl+O";
				return action;
			}

			inline App::UI::DescAction openRecentAction()
			{
				App::UI::DescAction action;
				action.key	= OPEN_RECENT;
				action.name = "Open recent";
				action.tip	= "Open a recent document";
				action.icon = "sprite/file/open_recent.png";
				return action;
			}

			inline App::UI::DescAction saveAction()
			{
				App::UI::DescAction action;
				action.key		= SAVE;
				action.name		= "Save";
				action.tip		= "Save project";
				action.icon		= Style::Icons::SAVE;
				action.shortcut = "Ctrl+S";
				return action;
			}

			inline App::UI::DescAction saveAsAction()
			{
				App::UI::DescAction action;
				action.key		= SAVE_AS;
				action.name		= "Save as...";
				action.tip		= "Copy project in a new save";
				action.icon		= Style::Icons::SAVE_AS;
				action.shortcut = "Ctrl+Shift+S";
				return action;
			}

			inline App::UI::DescAction importAction()
			{
				App::UI::DescAction action;
				action.key	= IMPORT;
				action.name = "Import";
				return action;
			}

			inline App::UI::DescAction exportAction()
			{
				App::UI::DescAction action;
				action.key	= EXPORT;
				action.name = "Export";
				return action;
			}

			inline App::UI::DescAction quitAction()
			{
				App::UI::DescAction action;
				action.key		= QUIT;
				action.name		= "Quit";
				action.tip		= "Exit software";
				action.icon		= Style::Icons::QUIT;
				action.shortcut = "Esc";
				return action;
			}
		} // namespace System

		namespace Camera
		{
			inline constexpr std::string_view ORTHOGRAPHIC = "camera.orthographic";
			inline constexpr std::string_view PERSPECTIVE  = "camera.perspective";
			inline constexpr std::string_view TRACKBALL	   = "camera.trackball";
			inline constexpr std::string_view FREEFLY	   = "camera.freefly";
			inline constexpr std::string_view ORIENT	   = "camera.orient";
			inline constexpr std::string_view RESET		   = "camera.reset";

			inline App::UI::DescAction orthographicAction()
			{
				App::UI::DescAction action;
				action.key		= ORTHOGRAPHIC;
				action.name		= "Orthographic";
				action.tip		= "Change camera projection mode";
				action.icon		= Style::Icons::CAMERA_ORTHOGRAPHIC;
				action.shortcut = "Alt+O";
				action.group	= "CameraProjection";
				return action;
			}

			inline App::UI::DescAction perspectiveAction()
			{
				App::UI::DescAction action;
				action.key		= PERSPECTIVE;
				action.name		= "Perspective";
				action.tip		= "Change camera projection mode";
				action.icon		= Style::Icons::CAMERA_PERSPECTIVE;
				action.shortcut = "Alt+P";
				action.group	= "CameraProjection";
				return action;
			}

			inline App::UI::DescAction trackballAction()
			{
				App::UI::DescAction action;
				action.key		= TRACKBALL;
				action.name		= "Trackball";
				action.tip		= "Use Trackball controller";
				action.icon		= Style::Icons::CONTROLLER_TRACKBALL;
				action.shortcut = "Alt+T";
				action.group	= "CameraController";
				return action;
			}

			inline App::UI::DescAction freeflyAction()
			{
				App::UI::DescAction action;
				action.key		= FREEFLY;
				action.name		= "Freefly";
				action.tip		= "Use Freefly controller";
				action.icon		= Style::Icons::CONTROLLER_FREEFLY;
				action.shortcut = "Alt+F";
				action.group	= "CameraController";
				return action;
			}

			inline App::UI::DescAction orientAction()
			{
				App::UI::DescAction action;
				action.key	= ORIENT;
				action.name = "Orient";
				action.tip	= "Orient camera on selection";
				action.icon = Style::Icons::CAMERA_ORIENT;
				return action;
			}

			inline App::UI::DescAction resetAction()
			{
				App::UI::DescAction action;
				action.key	= RESET;
				action.name = "Reset";
				action.tip	= "Reset camera";
				action.icon = Style::Icons::CAMERA_RESET;
				return action;
			}
		} // namespace Camera

		namespace Snapshot
		{
			inline constexpr std::string_view SNAPSHOT = "snapshot.snapshot";
			inline constexpr std::string_view EXPORT   = "snapshot.export";

			inline App::UI::DescAction snapshotAction()
			{
				App::UI::DescAction action;
				action.key		= SNAPSHOT;
				action.name		= "Snapshot";
				action.tip		= "Save current image";
				action.icon		= Style::Icons::SNAPSHOT;
				action.shortcut = "F2";
				return action;
			}

			inline App::UI::DescAction exportAction()
			{
				App::UI::DescAction action;
				action.key		= EXPORT;
				action.name		= "Export";
				action.tip		= "Open dialog to export image";
				action.icon		= Style::Icons::SNAPSHOT_EXPORT;
				action.shortcut = "F3";
				return action;
			}
		} // namespace Snapshot

		namespace Theme
		{
			inline constexpr std::string_view SET		   = "theme.set";
			inline constexpr std::string_view RESET_LAYOUT = "theme.resetLayout";
			inline constexpr std::string_view SET_FONT	   = "theme.setFont";
			inline constexpr std::string_view PARAM_THEME  = "theme";

			inline App::UI::DescAction setAction()
			{
				App::UI::DescAction action;
				action.key	= SET;
				action.name = "Set theme";
				action.tip	= "Change application theme";
				return action;
			}

			inline App::UI::DescAction resetLayoutAction()
			{
				App::UI::DescAction action;
				action.key	= RESET_LAYOUT;
				action.name = "Reset layout";
				return action;
			}

			inline App::UI::DescAction setFontAction()
			{
				App::UI::DescAction action;
				action.key	= SET_FONT;
				action.name = "Set font";
				action.tip	= "Change application font";
				return action;
			}
		} // namespace Theme

		namespace Help
		{
			inline constexpr std::string_view DOCUMENTATION = "help.documentation";
			inline constexpr std::string_view REPORT		= "help.report";
			inline constexpr std::string_view CHECK_UPDATES = "help.checkUpdates";
			inline constexpr std::string_view ABOUT			= "help.about";

			inline App::UI::DescAction documentationAction()
			{
				App::UI::DescAction action;
				action.key	= DOCUMENTATION;
				action.name = "Documentation";
				action.tip	= "Open online full documentation";
				action.icon = Style::Icons::DOCUMENTATION;
				return action;
			}

			inline App::UI::DescAction reportAction()
			{
				App::UI::DescAction action;
				action.key	= REPORT;
				action.name = "Report a bug";
				action.tip	= "Report a bug or ask for a feature";
				action.icon = Style::Icons::BUG;
				return action;
			}

			inline App::UI::DescAction checkUpdatesAction()
			{
				App::UI::DescAction action;
				action.key	= CHECK_UPDATES;
				action.name = "Check for updates";
				action.tip	= "Check online for a new version ";
				action.icon = Style::Icons::UPDATE;
				return action;
			}

			inline App::UI::DescAction aboutAction()
			{
				App::UI::DescAction action;
				action.key	= ABOUT;
				action.name = "About";
				action.icon = Style::Icons::INFO;
				return action;
			}
		} // namespace Help

		namespace Selection
		{
			inline constexpr std::string_view LOCK					  = "selection.lock";
			inline constexpr std::string_view SAVE					  = "selection.save";
			inline constexpr std::string_view CLEAR					  = "selection.clear";
			inline constexpr std::string_view SELECT_ALL			  = "selection.selectAll";
			inline constexpr std::string_view SET_GRANULARITY_SYSTEM  = "selection.setGranularitySystem";
			inline constexpr std::string_view SET_GRANULARITY_CHAIN	  = "selection.setGranularityChain";
			inline constexpr std::string_view SET_GRANULARITY_RESIDUE = "selection.setGranularityResidue";
			inline constexpr std::string_view SET_GRANULARITY_ATOM	  = "selection.setGranularityAtom";
			inline constexpr std::string_view SHOW					  = "selection.show";
			inline constexpr std::string_view HIDE					  = "selection.hide";
			inline constexpr std::string_view SOLO					  = "selection.solo";
			inline constexpr std::string_view SET_COLOR_SCHEME		  = "selection.setColorScheme";
			inline constexpr std::string_view SET_REPRESENTATION	  = "selection.setRepresentation";
			inline constexpr std::string_view DELETE				  = "selection.delete";
			inline constexpr std::string_view PARAM_COLOR_SCHEME	  = "colorScheme";
			inline constexpr std::string_view PARAM_COLOR_INDEX		  = "colorIndex";
			inline constexpr std::string_view PARAM_REPRESENTATION	  = "representation";

			inline App::UI::DescAction lockAction()
			{
				App::UI::DescAction action;
				action.key		 = LOCK;
				action.name		 = "Lock";
				action.tip		 = "Lock the current selection";
				action.icon		 = Style::Icons::LOCK;
				action.checkable = true;
				return action;
			}

			inline App::UI::DescAction saveAction()
			{
				App::UI::DescAction action;
				action.key	= SAVE;
				action.name = "Save";
				action.tip	= "Save the current selection";
				return action;
			}

			inline App::UI::DescAction clearAction()
			{
				App::UI::DescAction action;
				action.key	= CLEAR;
				action.name = "Clear";
				action.tip	= "Clear selection";
				return action;
			}

			inline App::UI::DescAction selectAllAction()
			{
				App::UI::DescAction action;
				action.key		= SELECT_ALL;
				action.name		= "Select all";
				action.tip		= "Select all systems";
				action.shortcut = "Ctrl+A";
				return action;
			}

			inline App::UI::DescAction setGranularitySystemAction()
			{
				App::UI::DescAction action;
				action.key	 = SET_GRANULARITY_SYSTEM;
				action.name	 = "System";
				action.tip	 = "Set selection granularity to system";
				action.icon	 = Style::Icons::SYSTEM;
				action.group = "SelectionGranularity";
				return action;
			}

			inline App::UI::DescAction setGranularityChainAction()
			{
				App::UI::DescAction action;
				action.key	 = SET_GRANULARITY_CHAIN;
				action.name	 = "Chain";
				action.tip	 = "Set selection granularity to chain";
				action.icon	 = Style::Icons::CHAIN;
				action.group = "SelectionGranularity";
				return action;
			}

			inline App::UI::DescAction setGranularityResidueAction()
			{
				App::UI::DescAction action;
				action.key	 = SET_GRANULARITY_RESIDUE;
				action.name	 = "Residue";
				action.tip	 = "Set selection granularity to residue";
				action.icon	 = Style::Icons::RESIDUE;
				action.group = "SelectionGranularity";
				return action;
			}

			inline App::UI::DescAction setGranularityAtomAction()
			{
				App::UI::DescAction action;
				action.key	 = SET_GRANULARITY_ATOM;
				action.name	 = "Atom";
				action.tip	 = "Set selection granularity to atom";
				action.icon	 = Style::Icons::ATOM;
				action.group = "SelectionGranularity";
				return action;
			}

			inline App::UI::DescAction showAction()
			{
				App::UI::DescAction action;
				action.key		= SHOW;
				action.name		= "Show";
				action.tip		= "Show selection";
				action.icon		= Style::Icons::VISIBILITY;
				action.shortcut = "Ctrl+Alt+S";
				return action;
			}

			inline App::UI::DescAction hideAction()
			{
				App::UI::DescAction action;
				action.key		= HIDE;
				action.name		= "Hide";
				action.tip		= "Hide selection";
				action.icon		= Style::Icons::VISIBILITY_OFF;
				action.shortcut = "Ctrl+Alt+H";
				return action;
			}

			inline App::UI::DescAction soloAction()
			{
				App::UI::DescAction action;
				action.key	= SOLO;
				action.name = "Solo";
				action.tip	= "Hide all but selection";
				action.icon = Style::Icons::VISIBILITY;
				return action;
			}

			inline App::UI::DescAction setColorSchemeAction()
			{
				App::UI::DescAction action;
				action.key	= SET_COLOR_SCHEME;
				action.name = "Set color scheme";
				action.tip	= "Change selection color scheme";
				action.icon = Style::Icons::COLOR_LAYOUT;
				return action;
			}

			inline App::UI::DescAction setRepresentationAction()
			{
				App::UI::DescAction action;
				action.key	= SET_REPRESENTATION;
				action.name = "Set representation";
				action.tip	= "Change selection representation";
				action.icon = Style::Icons::REPRESENTATION;
				return action;
			}

			inline App::UI::DescAction deleteAction()
			{
				App::UI::DescAction action;
				action.key		= DELETE;
				action.name		= "Delete";
				action.tip		= "Delete system";
				action.icon		= Style::Icons::DELETE;
				action.shortcut = "Del";
				return action;
			}
		} // namespace Selection

		namespace Option::Cache
		{
			inline constexpr std::string_view OPEN	  = "option.cache.open";
			inline constexpr std::string_view CLEAR	  = "option.cache.clear";
			inline constexpr std::string_view REFRESH = "option.cache.refresh";

			inline App::UI::DescAction openAction()
			{
				App::UI::DescAction action;
				action.key	= OPEN;
				action.name = "Open";
				action.tip	= "Open the cache folder in explorer";
				return action;
			}

			inline App::UI::DescAction clearAction()
			{
				App::UI::DescAction action;
				action.key	= CLEAR;
				action.name = "Clear";
				action.tip	= "Delete all cached files";
				return action;
			}

			inline App::UI::DescAction refreshAction()
			{
				App::UI::DescAction action;
				action.key	= REFRESH;
				action.name = "Refresh";
				action.tip	= "Refresh cache size";
				return action;
			}
		} // namespace Option::Cache
	} // namespace Action

} // namespace VTX::UI::QT

#endif
