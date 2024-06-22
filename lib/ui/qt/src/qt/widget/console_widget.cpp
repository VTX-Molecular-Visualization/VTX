#include "qt/widget/console_widget.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/widget/console/panel.hpp"
#include "qt/widget_factory.hpp"
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
