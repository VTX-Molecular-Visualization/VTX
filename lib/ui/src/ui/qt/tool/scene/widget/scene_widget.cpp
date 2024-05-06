#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include <iostream>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	SceneWidget::SceneWidget( QWidget * p_parent ) {}

	void SceneWidget::localize() { setWindowTitle( "Scene" ); };

	void SceneWidget::_setupUi( const QString & p_name ) {}

	void SceneWidget::_setupSlots() {}

} // namespace VTX::UI::QT::Tool::Scene::Widget
