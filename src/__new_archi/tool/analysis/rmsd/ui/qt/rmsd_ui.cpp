#include "rmsd_ui.hpp"
#include "__new_archi/ui/qt/application.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include "ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	RMSDTool::RMSDTool() {}

	void RMSDTool::instantiateTool()
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget & tooltab
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget &>(
				VTX::UI::QT::QT_APP()->getMainWindow().getMainMenu().getTab( layoutData.tabName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
				tooltab.getToolBlock( layoutData.blockName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const rmsdButton
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "rmsdButton" );
		rmsdButton->setData( "RMSD", ":/sprite/analysis_compute_rmsd_icon.png", Qt::Orientation::Vertical );

		toolBlock.pushButton( *rmsdButton );
		// toolBlock.validate();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

} // namespace VTX::Tool::Analysis::RMSD::UI::QT
