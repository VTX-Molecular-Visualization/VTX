#include <example/tool.hpp>
#include <qt/application.hpp>
#include <util/logger.hpp>

namespace VTX::Tool::Example
{
	Tool::Tool() {}

	Tool::~Tool() {}

	void Tool::init() { VTX_DEBUG( "Tool::init" ); }

	void Tool::onAppStart() { VTX_DEBUG( "Tool::onAppStart" ); }

	void Tool::createUI()
	{
		VTX_DEBUG( "Tool::createUI" );

		App::UI::DescAction action;
		action.name	   = "Example";
		action.trigger = [ this ]() { VTX_DEBUG( "TRIGGER" ); };

		// Add a menu action.
		APP_QT::addMenuAction( "Tool", action );
		// Add a toolbar action.
		APP_QT::addToolBarAction( "Tool", action );
	}

	void Tool::onAppStop() { VTX_DEBUG( "Tool::onAppStop" ); }
} // namespace VTX::Tool::Example
