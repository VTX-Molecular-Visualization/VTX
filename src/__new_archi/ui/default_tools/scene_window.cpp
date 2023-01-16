#include "scene_window.hpp"
#include "__new_archi/ui/default_tools/keys.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/widget/scene/contextual_menu_scene.hpp"
#include "__new_archi/ui/qt/widget/scene/view/molecule_scene_view.hpp"
#include "__new_archi/ui/qt/widget/scene/view/path_scene_view.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::DefaultTools
{
	QT::Widget::Scene::SceneItemWidget * SceneWindow::MoleculeInstancier::instantiateItem(
		Generic::BaseSceneItem * const p_item )
	{
		QT::Widget::Scene::SceneWidget * const sceneWidget
			= QT::QT_APP()->getMainWindow().getPanel<QT::Widget::Scene::SceneWidget>( DefaultTools::SCENE_WINDOW_KEY );

		Model::Molecule * const molecule = static_cast<Model::Molecule *>( p_item );

		QT::Widget::Scene::View::MoleculeSceneView * const moleculeSceneItemWidget
			= QT::WidgetFactory::get().instantiateViewWidget<QT::Widget::Scene::View::MoleculeSceneView>(
				molecule, ID::View::UI_MOLECULE_STRUCTURE, sceneWidget, "moleculeSceneView" );

		return moleculeSceneItemWidget;
	}

	void SceneWindow::MoleculeInstancier::destroyItem( const QT::Widget::Scene::SceneItemWidget * p_sceneWidget )
	{
		const QT::Widget::Scene::View::MoleculeSceneView * const moleculeSceneItemWidget
			= dynamic_cast<const QT::Widget::Scene::View::MoleculeSceneView *>( p_sceneWidget );

		const Model::Molecule & molecule
			= MVC::MvcManager::get().getModel<Model::Molecule>( moleculeSceneItemWidget->getModelID() );

		MVC::MvcManager::get().deleteView<QT::Widget::Scene::View::MoleculeSceneView>(
			&molecule, ID::View::UI_MOLECULE_STRUCTURE );
	}

	QT::Widget::Scene::SceneItemWidget * SceneWindow::PathInstancier::instantiateItem(
		Generic::BaseSceneItem * const p_item )
	{
		QT::Widget::Scene::SceneWidget * const sceneWidget
			= QT::QT_APP()->getMainWindow().getPanel<QT::Widget::Scene::SceneWidget>( DefaultTools::SCENE_WINDOW_KEY );

		Model::Path * const path = static_cast<Model::Path *>( p_item );

		QT::Widget::Scene::View::PathSceneView * const pathSceneItemWidget
			= QT::WidgetFactory::get().instantiateViewWidget<QT::Widget::Scene::View::PathSceneView>(
				path, ID::View::UI_SCENE_PATH, sceneWidget, "pathSceneView" );

		return pathSceneItemWidget;
	}

	void SceneWindow::PathInstancier::destroyItem( const QT::Widget::Scene::SceneItemWidget * p_sceneWidget )
	{
		const QT::Widget::Scene::View::PathSceneView * const pathSceneItemWidget
			= dynamic_cast<const QT::Widget::Scene::View::PathSceneView *>( p_sceneWidget );

		const Model::Path & path = MVC::MvcManager::get().getModel<Model::Path>( pathSceneItemWidget->getModelID() );

		MVC::MvcManager::get().deleteView<QT::Widget::Scene::View::PathSceneView>( &path, ID::View::UI_SCENE_PATH );
	}

	SceneWindow::SceneWindow() {}

	void SceneWindow::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Scene::SceneWidget * const sceneWidget
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Scene::SceneWidget>( mainWindow, "sceneWidget" );

		mainWindow->referencePanel( DefaultTools::SCENE_WINDOW_KEY, sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Horizontal );

		_registerDefaultObjects( *sceneWidget );
		_instantiateObjectsInScene( *sceneWidget, VTXApp::get().getScene() );

		setupContextualMenu();
	}

	void SceneWindow::_registerDefaultObjects( QT::Widget::Scene::SceneWidget & p_sceneWidget ) const
	{
		p_sceneWidget.registerSceneItemType( ID::Model::MODEL_MOLECULE, new MoleculeInstancier() );
		p_sceneWidget.registerSceneItemType( ID::Model::MODEL_PATH, new PathInstancier() );
	}

	void SceneWindow::_instantiateObjectsInScene( QT::Widget::Scene::SceneWidget & p_sceneWidget,
												  const VTX::Object3D::Scene &	   p_scene ) const
	{
		for ( const std::pair<Model::Molecule *, float> pairMolecule : p_scene.getMolecules() )
		{
			p_sceneWidget.instantiateSceneItemWidget( pairMolecule.first );
		}

		for ( Model::Path * const path : p_scene.getPaths() )
		{
			p_sceneWidget.instantiateSceneItemWidget( path );
		}
	}

	void SceneWindow::setupContextualMenu() const
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::Scene::ContextualMenuScene * defaultContextualMenu
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::Scene::ContextualMenuScene>(
				mainWindow, "contextualMenuScene" );

		mainWindow->getContextualMenu().registerMenu( DefaultTools::ContextualMenu::SCENE, defaultContextualMenu );
	}

} // namespace VTX::UI::DefaultTools
