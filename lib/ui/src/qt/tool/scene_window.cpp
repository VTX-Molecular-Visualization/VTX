#include "ui/qt/tool/scene_window.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/scene/widget/contextual_menu_scene.hpp"
#include "ui/qt/tool/scene/widget/view/molecule_scene_view.hpp"
#include "ui/qt/tool/scene/widget/view/path_scene_view.hpp"
#include "ui/qt/widget/contextual_menu/contextual_menu_selection.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/video/path.hpp>

namespace VTX::UI::QT::Tool
{
	Scene::Widget::SceneItemWidget * SceneWindow::MoleculeInstancier::instantiateItem(
		App::Core::Scene::BaseSceneItem * const p_item )
	{
		Scene::Widget::SceneWidget * const sceneWidget
			= QT::QT_APP()->getMainWindow().getPanel<Scene::Widget::SceneWidget>( Tool::SCENE_WINDOW_KEY );

		App::Component::Chemistry::Molecule * const molecule = static_cast<App::Component::Chemistry::Molecule *>( p_item );

		Scene::Widget::View::MoleculeSceneView * const moleculeSceneItemWidget
			= QT::WidgetFactory::get().instantiateViewWidget<Scene::Widget::View::MoleculeSceneView>(
				molecule, ID::View::UI_MOLECULE_STRUCTURE, sceneWidget, "moleculeSceneView" );

		return moleculeSceneItemWidget;
	}

	void SceneWindow::MoleculeInstancier::destroyItem( const Scene::Widget::SceneItemWidget * p_sceneWidget )
	{
		const Scene::Widget::View::MoleculeSceneView * const moleculeSceneItemWidget
			= dynamic_cast<const Scene::Widget::View::MoleculeSceneView *>( p_sceneWidget );

		const App::Component::Chemistry::Molecule & molecule
			= VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( moleculeSceneItemWidget->getModelID() );

		VTX::MVC_MANAGER().deleteView<Scene::Widget::View::MoleculeSceneView>( &molecule,
																				   ID::View::UI_MOLECULE_STRUCTURE );
	}

	Scene::Widget::SceneItemWidget * SceneWindow::PathInstancier::instantiateItem(
		App::Core::Scene::BaseSceneItem * const p_item )
	{
		Scene::Widget::SceneWidget * const sceneWidget
			= QT::QT_APP()->getMainWindow().getPanel<Scene::Widget::SceneWidget>( Tool::SCENE_WINDOW_KEY );

	 App::Component::Video::Path * const path = static_cast<App::Component::Video::Path *>( p_item );

		Scene::Widget::View::PathSceneView * const pathSceneItemWidget
			= QT::WidgetFactory::get().instantiateViewWidget<Scene::Widget::View::PathSceneView>(
				path, ID::View::UI_SCENE_PATH, sceneWidget, "pathSceneView" );

		return pathSceneItemWidget;
	}

	void SceneWindow::PathInstancier::destroyItem( const Scene::Widget::SceneItemWidget * p_sceneWidget )
	{
		const Scene::Widget::View::PathSceneView * const pathSceneItemWidget
			= dynamic_cast<const Scene::Widget::View::PathSceneView *>( p_sceneWidget );

		const App::Component::Video::Path & path = VTX::MVC_MANAGER().getModel<App::Component::Video::Path>( pathSceneItemWidget->getModelID() );

		VTX::MVC_MANAGER().deleteView<Scene::Widget::View::PathSceneView>( &path, ID::View::UI_SCENE_PATH );
	}

	SceneWindow::SceneWindow() : BaseQtTool() {}

	void SceneWindow::instantiateTool()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		Scene::Widget::SceneWidget * const sceneWidget
			= QT::WidgetFactory::get().instantiateWidget<Scene::Widget::SceneWidget>( mainWindow, "sceneWidget" );

		mainWindow->referencePanel( Tool::SCENE_WINDOW_KEY, sceneWidget );
		mainWindow->addDockWidgetAsTabified(
			sceneWidget, Qt::DockWidgetArea::LeftDockWidgetArea, Qt::Orientation::Horizontal );

		_registerDefaultObjects( *sceneWidget );
		_instantiateObjectsInScene( *sceneWidget, VTXApp::get().getScene() );

		setupContextualMenu();
	}

	void SceneWindow::_registerDefaultObjects( Scene::Widget::SceneWidget & p_sceneWidget ) const
	{
		p_sceneWidget.registerSceneItemType( ID::Model::MODEL_MOLECULE, new MoleculeInstancier() );
		p_sceneWidget.registerSceneItemType( ID::Model::MODEL_PATH, new PathInstancier() );
	}

	void SceneWindow::_instantiateObjectsInScene( Scene::Widget::SceneWidget & p_sceneWidget,
												  const VTX::App::Application::Scene & p_scene ) const
	{
		for ( const std::pair<App::Component::Chemistry::Molecule *, float> pairMolecule : p_scene.getMolecules() )
		{
			p_sceneWidget.instantiateSceneItemWidget( pairMolecule.first );
		}

		for ( App::Component::Video::Path * const path : p_scene.getPaths() )
		{
			p_sceneWidget.instantiateSceneItemWidget( path );
		}
	}

	void SceneWindow::setupContextualMenu() const
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		Scene::Widget::ContextualMenuScene * defaultContextualMenu
			= QT::WidgetFactory::get().instantiateWidget<Scene::Widget::ContextualMenuScene>( mainWindow,
																							  "contextualMenuScene" );
		mainWindow->getContextualMenu().registerMenu( Tool::ContextualMenu::SCENE, defaultContextualMenu );

		QT::Widget::ContextualMenu::ContextualMenuSelection * contextuaMenuSelection
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::ContextualMenu::ContextualMenuSelection>(
				mainWindow, "contextualMenuSelection" );
		mainWindow->getContextualMenu().registerMenu( Tool::ContextualMenu::SELECTION, contextuaMenuSelection );
	}

} // namespace VTX::UI::QT::Tool
