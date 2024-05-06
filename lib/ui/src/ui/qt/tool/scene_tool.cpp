#include "ui/qt/tool/scene_tool.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Tool
{
	SceneTool::SceneTool() : BaseQtTool() {}

	void SceneTool::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Tool::Scene::Widget::SceneWidget * const sceneWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Tool::Scene::Widget::SceneWidget>(
				mainWindow, "sceneWidget"
			);

		mainWindow->referencePanel( SCENE_PANEL_KEY, sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Vertical, true
		);
	}

} // namespace VTX::UI::QT::Tool
