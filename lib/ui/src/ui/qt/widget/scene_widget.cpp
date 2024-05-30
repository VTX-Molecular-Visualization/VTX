#include "ui/qt/widget/scene_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget/scene/panel.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>

namespace VTX::UI::QT::Widget
{
	SceneWidget::SceneWidget() : BaseQtTool() {}

	void SceneWidget::instantiateTool()
	{
		// Add panel.
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Scene::Panel * const sceneWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Scene::Panel>( mainWindow, "sceneWidget" );

		mainWindow->referencePanel( SCENE_PANEL_KEY, sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Vertical, true
		);

		// Connect callbacks.
		App::SCENE().onSceneItemAdded += [ sceneWidget ]( const App::Component::Scene::SceneItemComponent & p_item )
		{
			if ( App::MAIN_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) )
			{
				sceneWidget->getTreeWidget()->addTopLevelMolecule( p_item );
				sceneWidget->getTreeWidget()->addTopLevelMolecule( p_item );
				sceneWidget->getTreeWidget()->addTopLevelMolecule( p_item );
				sceneWidget->getTreeWidget()->addTopLevelMolecule( p_item );
				sceneWidget->getTreeWidget()->addTopLevelMolecule( p_item );
			}
		}; // namespace VTX::UI::QT::Widget
	}

} // namespace VTX::UI::QT::Widget
