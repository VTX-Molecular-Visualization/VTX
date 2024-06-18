#include "widget/console_widget.hpp"
#include "application_qt.hpp"
#include "main_window.hpp"
#include "widget/console/panel.hpp"
#include "widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	ConsoleWidget::ConsoleWidget()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Console::Panel * const consoleWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Console::Panel>( mainWindow, "consoleWidget" );

		mainWindow->referencePanel( ConsoleWidget::CONSOLE_PANEL_KEY, consoleWidget );
		mainWindow->addDockWidgetAsTabified(
			consoleWidget, Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Vertical, true
		);
	}

} // namespace VTX::UI::QT::Widget
