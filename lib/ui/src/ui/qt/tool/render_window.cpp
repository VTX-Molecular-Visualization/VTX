#include "ui/qt/tool/render_window.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/widget_factory.hpp"

namespace VTX::UI::QT::Tool
{
	RenderWindow::RenderWindow() : BaseQtTool() {}

	void RenderWindow::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Tool::Render::Widget::RenderWidget * const renderWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Tool::Render::Widget::RenderWidget>(
				mainWindow, "renderWidget"
			);
		mainWindow->referencePanel( "RENDER_WINDOW_KEY", renderWidget );

		mainWindow->setCentralWidget( renderWidget );

		_setupContextualMenu();
	}

	void RenderWindow::_setupContextualMenu() const {}

} // namespace VTX::UI::QT::Tool
