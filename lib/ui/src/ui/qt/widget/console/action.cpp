#include "ui/qt/widget/console/action.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/console/panel.hpp"
#include "ui/qt/widget/console_widget.hpp"

namespace VTX::UI::QT::Widget::Console::Action
{
	void ClearConsole::execute()
	{
		Widget::Console::Panel * const consoleWidgetPtr
			= QT_APP()->getMainWindow().getPanel<Widget::Console::Panel>( Widget::ConsoleWidget::CONSOLE_PANEL_KEY );

		if ( consoleWidgetPtr == nullptr )
			return;

		consoleWidgetPtr->clearLogs();
	}
} // namespace VTX::UI::QT::Widget::Console::Action
