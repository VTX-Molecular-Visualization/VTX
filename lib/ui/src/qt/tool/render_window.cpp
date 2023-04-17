#include "ui/qt/tool/render_window.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/render/widget/contextual_menu_render.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Tool
{
	RenderWindow::RenderWindow() : BaseQtTool() {}

	void RenderWindow::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Tool::Render::Widget::RenderWidget * const renderWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Tool::Render::Widget::RenderWidget>( mainWindow,
																								  "renderWidget" );
		mainWindow->referencePanel( Tool::RENDER_WINDOW_KEY, renderWidget );

		renderWidget->displayOverlay( QT::Tool::Render::Widget::Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS,
									  QT::Tool::Render::Widget::Overlay::OVERLAY_ANCHOR::BOTTOM_CENTER );

		mainWindow->setCentralWidget( renderWidget );

		setupContextualMenu();
	}

	void RenderWindow::setupContextualMenu() const
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Tool::Render::Widget::ContextualMenuRender * defaultContextualMenu
			= QT::WidgetFactory::get().instantiateWidget<QT::Tool::Render::Widget::ContextualMenuRender>(
				mainWindow, "contextualMenuRender" );

		mainWindow->getContextualMenu().registerMenu( Tool::ContextualMenu::RENDER, defaultContextualMenu );
	}

} // namespace VTX::UI::QT::Tool
