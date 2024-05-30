#include "ui/qt/widget/scene_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/scene/panel.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	SceneWidget::SceneWidget() : BaseQtTool() {}

	void SceneWidget::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Scene::Panel * const sceneWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Scene::Panel>( mainWindow, "sceneWidget" );

		mainWindow->referencePanel( SCENE_PANEL_KEY, sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Vertical, true
		);
	}

} // namespace VTX::UI::QT::Widget
