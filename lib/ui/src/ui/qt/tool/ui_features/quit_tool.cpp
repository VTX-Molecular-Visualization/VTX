#include "ui/qt/tool/ui_features/quit_tool.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"
#include "ui/qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/application/system/action_manager.hpp>
#include <app/core/action/base_action.hpp>

namespace VTX::UI::QT::Tool::UIFeatures
{
	namespace Action
	{
		class Quit final : public App::Core::Action::BaseAction
		{
		  public:
			Quit() {};
			void execute() override
			{
				App::VTXApp::get().onEndOfFrameOneShot += []() { QT_APP()->softQuit(); };
			};
		};
	} // namespace Action

	QuitTool::QuitTool() : BaseQtTool() {}

	void QuitTool::instantiateTool()
	{
		_addButtonsInMainMenu();
		_addCommands();
	}

	void QuitTool::_addButtonsInMainMenu()
	{
		UI::Core::ToolLayoutData mainMenuLayoutData = UI::Core::ToolLayoutData();
		mainMenuLayoutData.tabName					= "File";
		mainMenuLayoutData.blockName				= "Windows";

		QT::MainWindow * const						mainWindow = &QT::QT_APP()->getMainWindow();
		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock  = mainWindow->getMainMenuToolBlock( mainMenuLayoutData );

		QT::Widget::MainMenu::MenuToolButtonWidget * const quitButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "quitButton"
			);
		quitButton->setData( "Quit", ":/sprite/exit_icon.png", Qt::Orientation::Vertical );
		quitButton->setTriggerAction( this, &QuitTool::_quit );

		toolBlock.pushButton( *quitButton );
	}

	void QuitTool::_quit() { App::VTX_ACTION().execute<Action::Quit>(); }

	namespace Quit
	{
		class Binder final : public PythonBinding::Binder
		{
		  public:
			void bind( PythonBinding::PyTXModule & p_vtxmodule ) override
			{
				PythonBinding::Wrapper::Module & commands = p_vtxmodule.commands();
				commands.bindAction<Action::Quit>( "quit", "Quit the application." );
			}
		};
	} // namespace Quit

	void QuitTool::_addCommands() const { PythonBinding::INTERPRETOR().addBinder<Quit::Binder>(); }

} // namespace VTX::UI::QT::Tool::UIFeatures
