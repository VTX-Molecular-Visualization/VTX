#include "tool/example/example_tool.hpp"
#include "tool/example/actions.hpp"
#include "tool/example/widget/my_dock_widget.hpp"
#include "tool/example/widget/my_dialog.hpp"
#include "tool/example/widget/my_menu.hpp"
#include "tool/example/widget/my_tool_bar.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/services.hpp"
#include <QFile>
#include <string_view>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	namespace
	{
		constexpr std::string_view ACTION_EXAMPLE = "example.tool.action";
	}

	ExampleTool::ExampleTool() = default;

	void ExampleTool::registerActions()
	{
		App::UI::DescAction action;
		action.key	   = ACTION_EXAMPLE;
		action.name	   = "Example";
		action.tip	   = "Example tool";
		action.icon	   = "sprite/king.png";

		UI::QT::UI_ACTIONS().registerAction(
			action,
			[ this ]( const UI::QT::ActionRegistry::ActionContext & )
			{
				VTX_DEBUG( "Action triggered" );
				// Implement logic.
			}
		);

		UI::QT::UI_ACTIONS().registerAction(
			Action::myAction1(),
			[]( const UI::QT::ActionRegistry::ActionContext & )
			{
				Widget::MyDialog dialog;
				dialog.exec();
			}
		);
		UI::QT::UI_ACTIONS().registerAction( Action::myAction2() );
		UI::QT::UI_ACTIONS().registerAction( Action::myAction3() );
		UI::QT::UI_ACTIONS().registerAction( Action::myAction4() );
	}

	void ExampleTool::buildUI()
	{
		//////////////////////////
		// Method 1:
		// Easy way to add a menu/toobar action.
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", ACTION_EXAMPLE );
		mainWindow.addToolBarAction( "Tool", ACTION_EXAMPLE );

		//////////////////////////
		// Method 2:
		// Add custom widgets.
		mainWindow.createMenu<Widget::MyMenu>();
		mainWindow.createToolBar<Widget::MyToolBar>();
		mainWindow.createDockWidget<Widget::MyDockWidget>( Qt::RightDockWidgetArea );
	}

} // namespace VTX::Tool::Example
