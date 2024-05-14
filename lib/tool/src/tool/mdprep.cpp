#include "tool/mdprep.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"
#include "ui/qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/application/system/action_manager.hpp>
#include <app/core/action/base_action.hpp>
#include <util/logger.hpp>

namespace VTX::Tool
{

	ToolMdprep::ToolMdprep() {}
	void ToolMdprep::instantiateTool()
	{
		VTX_INFO( "Instantiate ToolMdprep" );
		_addButtonsInMenu();
	}
	void ToolMdprep::_addButtonsInMenu()
	{
		UI::Core::ToolLayoutData mainMenuLayoutData = UI::Core::ToolLayoutData();
		mainMenuLayoutData.tabName					= "Tools";
		mainMenuLayoutData.blockName				= "MD";

		VTX::UI::QT::MainWindow * const						 mainWindow = &VTX::UI::QT::QT_APP()->getMainWindow();
		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= mainWindow->getMainMenuToolBlock( mainMenuLayoutData );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const button
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "MdPrepButton"
			);
		// button->setData( "MD prep", ":/sprite/info_button.png", Qt::Orientation::Vertical );
		button->setData( "MD prep", ":/sprite/icon_tool_mdprep_mainButton.png", Qt::Orientation::Vertical );
		button->setTriggerAction( this, &ToolMdprep::_openMdPrepWindow );

		toolBlock.pushButton( *button );
	}
	void ToolMdprep::_openMdPrepWindow() { VTX_INFO( "Opening ToolMdprep window" ); }

} // namespace VTX::Tool
