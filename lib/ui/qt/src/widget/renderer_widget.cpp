#include "widget/renderer_widget.hpp"
#include "application_qt.hpp"
#include "main_window.hpp"
#include "widget/renderer/panel.hpp"
#include "widget_factory.hpp"

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
