#include "ui/qt/widget/renderer_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/renderer/panel.hpp"
#include "ui/qt/widget_factory.hpp"

namespace VTX::UI::QT::Widget
{
	RendererWidget::RendererWidget()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Renderer::Panel * const renderWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Renderer::Panel>( mainWindow, "renderWidget" );
		mainWindow->referencePanel( "RENDER_WINDOW_KEY", renderWidget );

		mainWindow->setCentralWidget( renderWidget );

		_setupContextualMenu();
	}

	void RendererWidget::_setupContextualMenu() const {}

} // namespace VTX::UI::QT::Widget
