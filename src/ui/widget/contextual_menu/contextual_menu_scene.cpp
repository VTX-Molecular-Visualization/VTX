#include "contextual_menu_scene.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "ui/dialog.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuScene::ContextualMenuScene( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuScene ::~ContextualMenuScene() {}

	void ContextualMenuScene::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuScene::_setupSlots()
	{
		addAction( "Load Molecule", this, &ContextualMenuScene::_loadMoleculeAction );
		addAction( "Download Molecule", this, &ContextualMenuScene::_downloadMoleculeAction );
		addSeparator();
		addAction( "Clear", this, &ContextualMenuScene::_clearAction );
	}

	void ContextualMenuScene::localize() {}

	void ContextualMenuScene::_loadMoleculeAction() { UI::Dialog::openLoadMoleculeDialog( this ); }
	void ContextualMenuScene::_downloadMoleculeAction() { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuScene::_clearAction() { VTX_ACTION( new Action::Main::ResetScene() ); }

} // namespace VTX::UI::Widget::ContextualMenu
