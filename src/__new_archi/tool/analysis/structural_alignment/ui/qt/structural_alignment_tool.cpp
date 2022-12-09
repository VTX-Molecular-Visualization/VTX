#include "structural_alignment_tool.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include "ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"
#include "vtx_app.hpp"

namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
{
	StructuralAlignmentTool::StructuralAlignmentTool() {}

	void StructuralAlignmentTool::instantiateTool()
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::Widget::MainMenu::MenuTooltabWidget & tooltab
			= VTXApp::get().getMainWindow().getMainMenu().getTab( layoutData.tabName );

		VTX::UI::Widget::MainMenu::MenuToolBlockWidget & toolBlock = tooltab.getToolBlock( layoutData.blockName );

		VTX::UI::Widget::MainMenu::MenuToolButtonSubmenuWidget * const structuralAlignmentButton
			= VTX::UI::WidgetFactory::get().instantiateWidget<VTX::UI::Widget::MainMenu::MenuToolButtonSubmenuWidget>(
				&tooltab, "structuralAlignmentButton" );
		structuralAlignmentButton->setData(
			"Structural alignment", ":/sprite/structural_alignment_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::Widget::MainMenu::MenuToolButtonWidget * const structuralAlignmentAdvancedButton
			= VTX::UI::WidgetFactory::get().instantiateWidget<VTX::UI::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "structuralAlignmentAdvancedButton" );
		structuralAlignmentAdvancedButton->setData(
			"Alignment\nwindow", ":/sprite/structural_alignment_settings_icon.png", Qt::Orientation::Horizontal );

		toolBlock.pushButton( *structuralAlignmentButton, 0 );
		toolBlock.pushButton( *structuralAlignmentAdvancedButton, 0 );
		toolBlock.validate();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

} // namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
