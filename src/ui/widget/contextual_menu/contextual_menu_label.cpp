#include "contextual_menu_label.hpp"
#include "action/action_manager.hpp"
#include "action/label.hpp"
#include "model/path.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/measurement/angle_scene_view.hpp"
#include "view/ui/widget/measurement/dihedral_angle_scene_view.hpp"
#include "view/ui/widget/measurement/distance_scene_view.hpp"
#include "view/ui/widget/measurement/distance_to_cycle_scene_view.hpp"

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

	void ContextualMenuLabel::setTarget( Model::Label * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getDefaultName() ) );
	}

	void ContextualMenuLabel::_orientAction() { VTX_ACTION( new Action::Label::Orient( _target ) ); }
	void ContextualMenuLabel::_renameAction()
	{
		UI::Widget::Scene::SceneItemWidget * sceneItem;

		if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE )
		{
			sceneItem = MVC::MvcManager::get().getView<View::UI::Widget::Measurement::DistanceSceneView>(
				_target, ID::View::UI_SCENE_DISTANCE_LABEL );
		}
		else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DISTANCE_TO_CYCLE )
		{
			sceneItem = MVC::MvcManager::get().getView<View::UI::Widget::Measurement::DistanceToCycleSceneView>(
				_target, ID::View::UI_SCENE_DISTANCE_TO_CYCLE_LABEL );
		}
		else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_ANGLE )
		{
			sceneItem = MVC::MvcManager::get().getView<View::UI::Widget::Measurement::AngleSceneView>(
				_target, ID::View::UI_SCENE_ANGLE_LABEL );
		}
		else if ( _target->getTypeId() == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
		{
			sceneItem = MVC::MvcManager::get().getView<View::UI::Widget::Measurement::DihedralAngleSceneView>(
				_target, ID::View::UI_SCENE_DIHEDRAL_ANGLE_LABEL );
		}
		else
		{
			sceneItem = nullptr;
		}

		if ( sceneItem != nullptr )
		{
			sceneItem->openRenameEditor( _target->getId() );
		}
	}
	void ContextualMenuLabel::_deleteAction() { VTX_ACTION( new Action::Label::Delete( _target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
