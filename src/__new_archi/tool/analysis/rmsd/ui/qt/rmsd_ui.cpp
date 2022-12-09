#include "rmsd_ui.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include "ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"
#include "vtx_app.hpp"

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	RMSDTool::RMSDTool() {}

	void RMSDTool::instantiateTool()
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::Widget::MainMenu::MenuTooltabWidget & tooltab
			= VTXApp::get().getMainWindow().getMainMenu().getTab( layoutData.tabName );

		VTX::UI::Widget::MainMenu::MenuToolBlockWidget & toolBlock = tooltab.getToolBlock( layoutData.blockName );

		VTX::UI::Widget::MainMenu::MenuToolButtonWidget * const rmsdButton
			= VTX::UI::WidgetFactory::get().instantiateWidget<VTX::UI::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "rmsdButton" );
		rmsdButton->setData( "RMSD", ":/sprite/analysis_compute_rmsd_icon.png", Qt::Orientation::Vertical );

		toolBlock.pushButton( *rmsdButton, 0 );
		toolBlock.validate();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

} // namespace VTX::Tool::Analysis::RMSD::UI::QT
