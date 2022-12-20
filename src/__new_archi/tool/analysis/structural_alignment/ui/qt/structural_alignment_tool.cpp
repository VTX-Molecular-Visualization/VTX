#include "structural_alignment_tool.hpp"
#include "__new_archi/ui/qt/application.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include "ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp"

namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
{
	StructuralAlignmentTool::StructuralAlignmentTool() {}

	void StructuralAlignmentTool::instantiateTool()
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget & tooltab
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget &>(
				VTX::UI::QT::QT_APP()->getMainWindow().getMainMenu().getTab( layoutData.tabName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
				tooltab.getToolBlock( layoutData.blockName ) );

		// VTX::UI::QT::Widget::MainMenu::MenuToolButtonSubmenuWidget * const structuralAlignmentButton
		//	= VTX::UI::WidgetFactory::get().instantiateWidget<VTX::UI::Widget::MainMenu::MenuToolButtonSubmenuWidget>(
		//		&tooltab, "structuralAlignmentButton" );
		// structuralAlignmentButton->setData(
		//	"Structural alignment", ":/sprite/structural_alignment_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const structuralAlignmentAdvancedButton
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "structuralAlignmentAdvancedButton" );

		structuralAlignmentAdvancedButton->setData(
			"Alignment\nwindow", ":/sprite/structural_alignment_settings_icon.png", Qt::Orientation::Horizontal );

		// toolBlock.pushButton( *structuralAlignmentButton, 0 );
		// toolBlock.pushButton( structuralAlignmentButton, structuralAlignmentAdvancedButton );
		toolBlock.pushButton( *structuralAlignmentAdvancedButton );
		// toolBlock.validate();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

} // namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
