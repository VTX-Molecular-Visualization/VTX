#include "structural_alignment_tool.hpp"
#include <old/ui/widget/main_menu/tool/menu_tool_structural_alignment_widget.hpp>
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <ui/qt/widget/main_menu/menu_toolbutton_submenu_widget.hpp>
#include <ui/qt/widget/main_menu/menu_toolbutton_widget.hpp>
#include <ui/qt/widget/main_menu/menu_tooltab_widget.hpp>
#include <ui/qt/widget_factory.hpp>

namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
{
	StructuralAlignmentTool::StructuralAlignmentTool() : VTX::UI::QT::BaseQtTool() {}

	void StructuralAlignmentTool::instantiateTool()
	{
		_addButtonsInMainMenu();

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

	void StructuralAlignmentTool::_addButtonsInMainMenu() const
	{
		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget & tooltab
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget &>(
				VTX::UI::QT::QT_APP()->getMainWindow().getMainMenu().getTab( layoutData.tabName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
				tooltab.getToolBlock( layoutData.blockName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonSubmenuWidget * const structuralAlignmentButton
			= VTX::UI::QT::WidgetFactory::get()
				  .instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonSubmenuWidget>(
					  &tooltab, "structuralAlignmentButton" );
		structuralAlignmentButton->setData(
			"Structural\nalignment", ":/sprite/structural_alignment_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const structuralAlignmentAdvancedButton
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "structuralAlignmentAdvancedButton" );

		structuralAlignmentAdvancedButton->setData(
			"Alignment\nwindow", ":/sprite/structural_alignment_settings_icon.png", Qt::Orientation::Horizontal );

		toolBlock.pushButton( *structuralAlignmentButton, *structuralAlignmentAdvancedButton );
	}

} // namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
