#include "tool/topology_editor/topology_editor.hpp"
#include "tool/topology_editor/dialog/topology_editor_dialog.hpp"
#include <QCursor>
#include <QMenu>
#include <QPointer>
#include <app/system/metadata.hpp>
#include <core/struct/topology.hpp>
#include <app/ui/concepts.hpp>
#include <string_view>
#include <ui/qt/action_registry.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/widget/main_window.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::TopologyEditor
{
	namespace
	{
		constexpr std::string_view ACTION_OPEN_TOPOLOGY_EDITOR = "tool.topology_editor.open";
		QPointer<Dialog::TopologyEditorDialog> g_dialog;

		QString entityLabel( const App::ECS::Entity p_entity )
		{
			const auto & metadata = App::REG().get<App::System::Metadata>( p_entity );
			if ( not metadata.name.empty() )
			{
				return QString::fromStdString( metadata.name );
			}
			if ( not metadata.pdbIDCode.empty() )
			{
				return QString::fromStdString( metadata.pdbIDCode );
			}
			if ( not metadata.path.empty() )
			{
				return QString::fromStdString( metadata.path.stem().string() );
			}
			return QString( "System %1" ).arg( static_cast<uint>( p_entity ) );
		}

		void openDialogForSystem( const App::ECS::Entity p_system )
		{
			if ( p_system == App::ECS::InvalidEntity )
			{
				VTX_WARNING( "Topology Editor: no system available" );
				return;
			}

			if ( g_dialog == nullptr || g_dialog->getSystem() != p_system )
			{
				if ( g_dialog != nullptr )
				{
					g_dialog->close();
					delete g_dialog;
				}
				g_dialog = new Dialog::TopologyEditorDialog( p_system );
				g_dialog->setAttribute( Qt::WA_DeleteOnClose, false );
			}

			g_dialog->show();
			g_dialog->raise();
			g_dialog->activateWindow();
		}

		App::UI::DescAction openTopologyEditorAction()
		{
			App::UI::DescAction action;
			action.key		= ACTION_OPEN_TOPOLOGY_EDITOR;
			action.name		= "Topology Editor";
			action.tip		= "Open Topology Editor";
			action.shortcut = "ctrl+alt+T";
			return action;
		}
	} // namespace

	TopologyEditor::TopologyEditor() = default;

	void TopologyEditor::registerActions()
	{
		UI::QT::UI_ACTIONS().registerAction(
			openTopologyEditorAction(),
			[]( const UI::QT::ActionRegistry::ActionContext & )
			{
				auto  systems = App::REG().view<Core::Struct::Topology, App::System::Metadata>();
				if ( systems.begin() == systems.end() )
				{
					VTX_WARNING( "Topology Editor: no system available" );
					return;
				}

				const App::ECS::Entity firstSystem = *systems.begin();
				auto				   secondIt	 = systems.begin();
				++secondIt;
				if ( secondIt == systems.end() )
				{
					openDialogForSystem( firstSystem );
					return;
				}

				QMenu menu( &UI::QT::MAIN_WINDOW() );
				for ( const App::ECS::Entity entity : systems )
				{
					QAction * const action = menu.addAction( entityLabel( entity ) );
					action->setData( static_cast<uint>( entity ) );
				}

				QAction * const selected = menu.exec( QCursor::pos() );
				if ( selected == nullptr )
				{
					return;
				}

				openDialogForSystem( App::ECS::Entity( selected->data().toUInt() ) );
			}
		);
	}

	void TopologyEditor::buildUI()
	{
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", ACTION_OPEN_TOPOLOGY_EDITOR );
		mainWindow.addToolBarAction( "Tool", ACTION_OPEN_TOPOLOGY_EDITOR );
	}
} // namespace VTX::Tool::TopologyEditor
