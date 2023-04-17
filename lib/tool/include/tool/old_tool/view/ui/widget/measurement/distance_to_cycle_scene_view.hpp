#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_TO_CYCLE_SCENE__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_TO_CYCLE_SCENE__

#include "tool/old_tool/model/measurement/distance_to_cycle.hpp"
#include <QMimeData>
#include <QWidget>
#include <app/old_app/id.hpp>
#include <app/old_app/view/base_view.hpp>
#include <string>
#include <ui/old_ui/ui/widget/scene/scene_item_widget.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	class DistanceToCycleSceneView :
		public View::BaseView<Model::Measurement::DistanceToCycle>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_MODEL

	  public:
		virtual const Model::ID &			   getModelID() const { return _model->getId(); };
		virtual const Generic::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		void notify( const Event::VTXEvent * const p_event ) override;

	  protected:
		DistanceToCycleSceneView( Model::Measurement::DistanceToCycle * const p_model, QWidget * const p_parent );
		void _setupSlots() override;

		void _fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection ) override;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

	  private:
		// void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif