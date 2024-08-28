#include "example/tool.hpp"
#include "example/widget/dock_widget.hpp"
#include "example/widget/menu.hpp"
#include "example/widget/tool_bar.hpp"
#include <qt/application.hpp>
#include <qt/dock_widget/inspector.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	Tool::Tool() {}

	Tool::~Tool() {}

	void Tool::init() {}

	void Tool::onAppStart() {}

	void Tool::createUI()
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
		APP_QT::getMainWindow()->createMenu<Widget::Menu>();
		APP_QT::getMainWindow()->createToolBar<Widget::ToolBar>();
		// TODO: hide this.
		auto * dockWidget = APP_QT::getMainWindow()->createDockWidget<Widget::DockWidget>( Qt::RightDockWidgetArea );
		auto * dockWidgetInspector = UI::QT::WIDGETS::get().get<UI::QT::DockWidget::Inspector *>();
		APP_QT::getMainWindow()->tabifyDockWidget( dockWidgetInspector, dockWidget );
	}

	void Tool::onAppStop() {}
} // namespace VTX::Tool::Example
