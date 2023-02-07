#include "contextual_menu_scene.hpp"
#include "qt/tool/session/dialog.hpp"
#include <old/action/action_manager.hpp>
#include <old/action/scene.hpp>
#include <old/action/viewpoint.hpp>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	ContextualMenuScene::ContextualMenuScene( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuScene ::~ContextualMenuScene() {}

	void ContextualMenuScene::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuScene::_setupSlots()
	{
		// addAction( "Load Molecule", this, &ContextualMenuScene::_loadMoleculeAction );
		// addAction( "Download Molecule", this, &ContextualMenuScene::_downloadMoleculeAction );
		// addSeparator();
		// addAction( "Show All", this, &ContextualMenuScene::_showAllMoleculesAction );
		// addSeparator();
		// addAction( "Add Viewpoint", this, &ContextualMenuScene::_addViewpointAction );
		// addSeparator();
		addSection( "Scene", SECTION_POSITION_HINT::BOTTOM );
		appendToSection( "Scene", "Clear", this, &ContextualMenuScene::_clearAction );
	}

	void ContextualMenuScene::localize() {}

	void ContextualMenuScene::_loadMoleculeAction() { QT::Tool::Session::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuScene::_downloadMoleculeAction() { QT::Tool::Session::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuScene::_showAllMoleculesAction() const { VTX_ACTION( new Action::Scene::ShowAllMolecules() ); }
	void ContextualMenuScene::_addViewpointAction() { VTX_ACTION( new Action::Viewpoint::Create() ); }
	void ContextualMenuScene::_clearAction() { VTX_ACTION( new Action::Scene::ResetScene() ); }

} // namespace VTX::UI::QT::Tool::Scene::Widget
