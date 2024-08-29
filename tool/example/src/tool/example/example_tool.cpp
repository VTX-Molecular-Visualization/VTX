#include "tool/example/example_tool.hpp"
#include "tool/example/widget/my_dock_widget.hpp"
#include "tool/example/widget/my_menu.hpp"
#include "tool/example/widget/my_tool_bar.hpp"
#include <ui/qt/application.hpp>
#include <ui/qt/dock_widget/inspector.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	ExampleTool::ExampleTool() {}

	ExampleTool::~ExampleTool() {}

	void ExampleTool::init() {}

	void ExampleTool::onAppStart() {}

	void ExampleTool::createUI()
	{
		//////////////////////////
		// Method 1:
		// Easy way to add a menu/toobar action.
		App::UI::DescAction action;
		action.name	   = "Example";
		action.tip	   = "Example tool";
		action.icon	   = "sprite/king.png";
		action.trigger = [ this ]()
		{
			VTX_DEBUG( "Action triggered" );
			// Implement logic.
			//
		};

		// Add to named button group.
		APP_QT::addMenuAction( "Tool", action );
		APP_QT::addToolBarAction( "Tool", action );

		//////////////////////////
		// Method 2:
		// Add custom widgets.
		APP_QT::getMainWindow()->createMenu<Widget::MyMenu>();
		APP_QT::getMainWindow()->createToolBar<Widget::MyToolBar>();
		// TODO: hide this.
		auto * dockWidget = APP_QT::getMainWindow()->createDockWidget<Widget::MyDockWidget>( Qt::RightDockWidgetArea );
		auto * dockWidgetInspector = UI::QT::WIDGETS::get().get<UI::QT::DockWidget::Inspector *>();
		APP_QT::getMainWindow()->tabifyDockWidget( dockWidgetInspector, dockWidget );
	}

	void ExampleTool::onAppStop() {}
} // namespace VTX::Tool::Example
