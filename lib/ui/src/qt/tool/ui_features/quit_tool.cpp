#include "ui/qt/tool/ui_features/quit_tool.hpp"
#include "ui/qt/action/main.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/old_app/action/action_manager.hpp>

namespace VTX::UI::QT::Tool::UIFeatures
{
	QuitTool::QuitTool() : BaseQtTool() {}

	void QuitTool::instantiateTool() { _addButtonsInMainMenu(); }

	void QuitTool::_addButtonsInMainMenu()
	{
		QT::MainWindow * const						mainWindow = &QT::QT_APP()->getMainWindow();
		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock  = mainWindow->getMainMenuToolBlock( getLayoutData() );

		QT::Widget::MainMenu::MenuToolButtonWidget * const quitButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>( &toolBlock,
																									  "quitButton" );
		quitButton->setData( "Quit", ":/sprite/exit_icon.png", Qt::Orientation::Vertical );
		quitButton->setTriggerAction( this, &QuitTool::_quit );

		toolBlock.pushButton( *quitButton );
	}

	void QuitTool::_quit() { VTX_ACTION( new Action::Main::Quit() ); }
} // namespace VTX::UI::QT::Tool::UIFeatures
