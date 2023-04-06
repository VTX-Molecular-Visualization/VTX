#include "contextual_menu_scene.hpp"

namespace VTX::UI::QT::Tool::Scene::Widget
{
	ContextualMenuScene::ContextualMenuScene( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuScene ::~ContextualMenuScene() {}

	void ContextualMenuScene::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuScene::_setupSlots() {}

	void ContextualMenuScene::localize() {}
} // namespace VTX::UI::QT::Tool::Scene::Widget
