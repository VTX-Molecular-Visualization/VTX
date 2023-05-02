#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_label.hpp"
#include "ui/old_ui/ui/widget/scene/scene_item_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
// #include "ui/old_ui/view/ui/widget/measurement/angle_scene_view.hpp"
// #include "ui/old_ui/view/ui/widget/measurement/dihedral_angle_scene_view.hpp"
// #include "ui/old_ui/view/ui/widget/measurement/distance_scene_view.hpp"
// #include "ui/old_ui/view/ui/widget/measurement/distance_to_cycle_scene_view.hpp"
#include "ui/qt/action/label.hpp"

#include <app/action/label.hpp>
#include <app/component/video/path.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuLabel::ContextualMenuLabel( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuLabel ::~ContextualMenuLabel() {}

	void ContextualMenuLabel::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuLabel::_setupSlots()
	{
		addAction( "Orient", this, &ContextualMenuLabel::_orientAction );
		addSection( "Edit" );
		addAction( "Rename", this, &ContextualMenuLabel::_renameAction );
		addAction( "Delete", this, &ContextualMenuLabel::_deleteAction );
	}

	void ContextualMenuLabel::localize() {}

	void ContextualMenuLabel::setTarget( App::Component::Object3D::Label * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getDefaultName() ) );
	}

	void ContextualMenuLabel::_orientAction() { VTX_ACTION( new QT::Action::Label::Orient( _target ) ); }
	void ContextualMenuLabel::_renameAction()
	{
		UI::Widget::Scene::SceneItemWidget * sceneItem = nullptr;

		// if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE )
		//{
		//	sceneItem = VTX::MVC_MANAGER().getView<View::UI::Widget::Measurement::DistanceSceneView>(
		//		_target, ID::View::UI_SCENE_DISTANCE_LABEL );
		// }
		// else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
		//{
		//	sceneItem = VTX::MVC_MANAGER().getView<View::UI::Widget::Measurement::DistanceToCycleSceneView>(
		//		_target, ID::View::UI_SCENE_DISTANCE_TO_CYCLE_LABEL );
		// }
		// else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_ANGLE )
		//{
		//	sceneItem = VTX::MVC_MANAGER().getView<View::UI::Widget::Measurement::AngleSceneView>(
		//		_target, ID::View::UI_SCENE_ANGLE_LABEL );
		// }
		// else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
		//{
		//	sceneItem = VTX::MVC_MANAGER().getView<View::UI::Widget::Measurement::DihedralAngleSceneView>(
		//		_target, ID::View::UI_SCENE_DIHEDRAL_ANGLE_LABEL );
		// }
		// else
		//{
		//	sceneItem = nullptr;
		// }

		if ( sceneItem != nullptr )
		{
			sceneItem->openRenameEditor( _target->getId() );
		}
	}
	void ContextualMenuLabel::_deleteAction() { VTX_ACTION( new App::Action::Label::Delete( _target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
