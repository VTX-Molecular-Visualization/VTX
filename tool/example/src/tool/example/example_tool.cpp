#include "tool/example/example_tool.hpp"
#include "tool/example/widget/my_dock_widget.hpp"
#include "tool/example/widget/my_menu.hpp"
#include "tool/example/widget/my_tool_bar.hpp"
#include "ui/qt/services.hpp"
#include <QFile>
#include <ui/qt/macros.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	ExampleTool::ExampleTool()
	{
		// Init assets.
		// Use the same target name as in the CMake file when using add_resources().
		VTX_INIT_RESOURCES( vtx_qt_resources_tool_example );

		//////////////////////////
		// Method 1:
		// Easy way to add a menu/toobar action.
		App::UI::DescAction action;
		action.key	   = "example.tool.action";
		action.name	   = "Example";
		action.tip	   = "Example tool";
		action.icon	   = "sprite/king.png";
		action.trigger = [ this ]()
		{
			VTX_DEBUG( "Action triggered" );
			// Implement logic.
		};

		// Add to named button group.
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", action );
		mainWindow.addToolBarAction( "Tool", action );

		//////////////////////////
		// Method 2:
		// Add custom widgets.
		mainWindow.createMenu<Widget::MyMenu>();
		mainWindow.createToolBar<Widget::MyToolBar>();
		mainWindow.createDockWidget<Widget::MyDockWidget>( Qt::RightDockWidgetArea );
	}

	std::optional<std::string> ExampleTool::getStyle() const
	{
		QFile stylesheetFile( ":/tool_example_style.css" );
		if ( stylesheetFile.open( QFile::ReadOnly ) )
		{
			return stylesheetFile.readAll().toStdString();
		}
		return std::nullopt;
	}

} // namespace VTX::Tool::Example
