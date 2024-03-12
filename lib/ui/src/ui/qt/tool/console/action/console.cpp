#include "ui/qt/tool/console/action/console.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/console/widget/console_widget.hpp"
#include "ui/qt/tool/console_tool.hpp"

namespace VTX::UI::QT::Tool::Console::Action
{
	void ClearConsole::execute()
	{
		Widget::ConsoleWidget * const consoleWidgetPtr
			= QT_APP()->getMainWindow().getPanel<Widget::ConsoleWidget>( ConsoleTool::CONSOLE_PANEL_KEY );

		if ( consoleWidgetPtr == nullptr )
			return;

		consoleWidgetPtr->clearLogs();
	}
} // namespace VTX::UI::QT::Tool::Console::Action
