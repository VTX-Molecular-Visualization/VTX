#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_SCENE__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_SCENE__

#include "id.hpp"
#include "model/measurement/angle.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QMimeData>
#include <QWidget>
#include <string>

namespace VTX::View::UI::Widget::Measurement
{
	class AngleSceneView :
		public View::BaseView<Model::Measurement::Angle>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_MODEL

	  public:
		virtual const Model::ID &			   getModelID() const { return _model->getId(); };
		virtual const Generic::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		void notify( const Event::VTXEvent * const p_event ) override;

	  protected:
		AngleSceneView( Model::Measurement::Angle * const p_model, QWidget * const p_parent );
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
