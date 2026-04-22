#include "tool/topology_editor/topology_editor.hpp"
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
			{ VTX_DEBUG( "Topology Editor action triggered" ); }
		);
	}

	void TopologyEditor::buildUI()
	{
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", ACTION_OPEN_TOPOLOGY_EDITOR );
		mainWindow.addToolBarAction( "Tool", ACTION_OPEN_TOPOLOGY_EDITOR );
	}
} // namespace VTX::Tool::TopologyEditor
