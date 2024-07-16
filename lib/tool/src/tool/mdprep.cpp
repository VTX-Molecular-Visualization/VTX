#include "ui/qt/application_qt.hpp"
#include "ui/qt/core/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/core/main_menu/menu_toolbutton_widget.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/pytx/include_python_binding.hpp"
#include "ui/qt/widget/ui_features/quit.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/application/system/action_manager.hpp>
#include <app/core/action/base_action.hpp>
#include <optional>
#include <util/logger.hpp>
//
#include "tool/mdprep.hpp"
#include "tools/mdprep/mdprep.hpp"

namespace VTX::Tool
{

	struct ToolMdprep::Data
	{
		std::optional<VTX::Tool::Mdprep::MainWindow> mainWindow = std::nullopt;
	};

	ToolMdprep::ToolMdprep() : _data( new Data() ) {}
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

		VTX::UI::QT::MainWindow * const					   mainWindow = &VTX::UI::QT::QT_APP()->getMainWindow();
		VTX::UI::QT::Core::MainMenu::MenuToolBlockWidget & toolBlock
			= mainWindow->getMainMenuToolBlock( mainMenuLayoutData );

		VTX::UI::QT::Core::MainMenu::MenuToolButtonWidget * const button
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Core::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "MdPrepButton"
			);
		button->setData( "MD prep", ":/sprite/icon_tool_mdprep_mainButton.png", Qt::Orientation::Vertical );
		button->setTriggerAction( this, &ToolMdprep::_openMdPrepWindow );

		toolBlock.pushButton( *button );
	}
	void ToolMdprep::_openMdPrepWindow()
	{
		VTX_INFO( "Opening ToolMdprep window" );
		_data->mainWindow.emplace();
		_data->mainWindow->show();
	}

	void VTX::Tool::ToolMdprep::Del::operator()( Data * p_ ) const noexcept { delete p_; }

} // namespace VTX::Tool
