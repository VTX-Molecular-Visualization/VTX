#include "ui/qt/widget/inspector_widget.hpp"
#include "ui/qt/widget/inspector/panel.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/application/scene.hpp>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::QT::Widget
{
	InspectorWidget::InspectorWidget() : BaseQtTool() {}

	void InspectorWidget::instantiateTool()
	{
		using namespace Inspector;
		
		// Add panel.
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		Panel * const inspectorWidget = QT::WidgetFactory::get().instantiateWidget<Panel>( mainWindow, "inspectorWidget" );

		mainWindow->referencePanel( INSPECTOR_PANEL_KEY, inspectorWidget );
		mainWindow->addDockWidgetAsTabified(
			inspectorWidget, Qt::DockWidgetArea::RightDockWidgetArea, Qt::Orientation::Vertical, true
		);

		// Connect callbacks.
		App::SCENE().onSceneItemAdded +=
			[ inspectorWidget ]( const App::Component::Scene::SceneItemComponent & p_item ) {
			//auto & molecule = inspectorGw->getMoleculeData( p_item );
			assert( App::MAIN_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) );
			inspectorWidget->setPItem(p_item);
			};
	} // namespace VTX::UI::QT::Widget
} // namespace VTX::UI::QT::Widget