#include "ui/qt/tool/console_tool.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/console/widget/console_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Tool
{
	ConsoleTool::ConsoleTool() : BaseQtTool() {}

	void ConsoleTool::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Tool::Console::Widget::ConsoleWidget * const consoleWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Tool::Console::Widget::ConsoleWidget>(
				mainWindow, "consoleWidget"
			);

		mainWindow->referencePanel( CONSOLE_PANEL_KEY, consoleWidget );
		mainWindow->addDockWidgetAsTabified(
			consoleWidget, Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Vertical, true
		);
	}

} // namespace VTX::UI::QT::Tool
