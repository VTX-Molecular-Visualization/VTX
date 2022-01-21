#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_SCENE__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DISTANCE_SCENE__

#include "id.hpp"
#include "model/measurement/distance.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QMimeData>
#include <QWidget>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	class DistanceSceneView :
		public View::BaseView<Model::Measurement::Distance>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_MODEL

	  public:
		virtual const Model::ID & getModelID() const { return _model->getId(); };

		void notify( const Event::VTXEvent * const p_event ) override;

	  protected:
		DistanceSceneView( Model::Measurement::Distance * const p_model, QWidget * const p_parent );
		void _setupSlots() override;

		QMimeData * _getDataForDrag() const override;

		void _fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection ) override;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

	  private:
		// void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
