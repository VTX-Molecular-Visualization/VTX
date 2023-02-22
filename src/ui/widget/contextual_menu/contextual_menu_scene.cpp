#include "contextual_menu_scene.hpp"
#include "action/action_manager.hpp"
#include "action/scene.hpp"
#include "action/viewpoint.hpp"
#include "ui/dialog.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuScene::ContextualMenuScene( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuScene ::~ContextualMenuScene() {}

	void ContextualMenuScene::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuScene::_setupSlots()
	{
		addSection( "Loading" );
		addAction( "Load Molecule", this, &ContextualMenuScene::_loadMoleculeAction );
		addAction( "Download Molecule", this, &ContextualMenuScene::_downloadMoleculeAction );

		addSection( "Actions" );
		addAction( "Show All", this, &ContextualMenuScene::_showAllMoleculesAction );
		addSection( "Viewpoints" );
		addAction( "Add Viewpoint", this, &ContextualMenuScene::_addViewpointAction );
		addSection( "Scene" );
		addAction( "Clear", this, &ContextualMenuScene::_clearAction );
	}

	void ContextualMenuScene::localize() {}

	void ContextualMenuScene::_loadMoleculeAction() { UI::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuScene::_downloadMoleculeAction() { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuScene::_showAllMoleculesAction() const { VTX_ACTION( new Action::Scene::ShowAllMolecules() ); }
	void ContextualMenuScene::_addViewpointAction() { VTX_ACTION( new Action::Viewpoint::Create() ); }
	void ContextualMenuScene::_clearAction() { VTX_ACTION( new Action::Scene::ResetScene() ); }

} // namespace VTX::UI::Widget::ContextualMenu
