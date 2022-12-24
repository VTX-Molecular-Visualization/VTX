#include "render_window.hpp"
#include "__new_archi/ui/default_tools/keys.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/widget/render/render_widget.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::DefaultTools
{
	RenderWindow::RenderWindow() {}

	void RenderWindow::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Render::RenderWidget * const renderWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Render::RenderWidget>( mainWindow,
																							"renderWidget" );
		mainWindow->referencePanel( DefaultTools::RENDER_WINDOW_KEY, renderWidget );

		renderWidget->displayOverlay( QT::Widget::Render::Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS,
									  QT::Widget::Render::Overlay::OVERLAY_ANCHOR::BOTTOM_CENTER );

		// QWidget * const		centralWidget = new QWidget( mainWindow );
		// QVBoxLayout * const layout		  = new QVBoxLayout( centralWidget );
		// layout->addWidget( renderWidget );
		// mainWindow->setCentralWidget( centralWidget );

		mainWindow->setCentralWidget( renderWidget );

		// VTXApp::get().getMainWindow().getContextualMenu().getMenu()
	}

} // namespace VTX::UI::DefaultTools
