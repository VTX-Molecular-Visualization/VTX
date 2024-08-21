#include <example/tool.hpp>
#include <qt/application.hpp>
#include <util/logger.hpp>
// #include "tool/

namespace VTX::Tool::Example
{
	Tool::Tool() {}

	Tool::~Tool() {}

	void Tool::init() {}

	void Tool::onAppStart() {}

	void Tool::createUI()
	{
		// Easy way to add a menu action.
		App::UI::DescAction action;
		action.name	   = "Example";
		action.trigger = [ this ]() {};

		APP_QT::addMenuAction( "Tool", action );
		APP_QT::addToolBarAction( "Tool", action );

		// Add a custom widget.
	}

	void Tool::onAppStop() {}
} // namespace VTX::Tool::Example
