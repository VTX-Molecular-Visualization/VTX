#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_SCENE__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_ANGLE_SCENE__

#include "tool/old_tool/model/measurement/angle.hpp"
#include <QMimeData>
#include <QWidget>
#include <app/id.hpp>
#include <app/core/view/base_view.hpp>
#include <string>
#include <ui/old_ui/ui/widget/scene/scene_item_widget.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	class AngleSceneView :
		public App::Core::View::BaseView<Model::Measurement::Angle>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_MODEL

	  public:
		virtual const App::Core::Model::ID &			   getModelID() const { return _model->getId(); };
		virtual const App::Core::Scene::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		void notify( const App::Core::Event::VTXEvent * const p_event ) override;

	  protected:
		AngleSceneView( Model::Measurement::Angle * const p_model, QWidget * const p_parent );
		void _setupSlots() override;

		void _fillItemSelection( const App::Application::Selection::SelectionModel & p_selection, QItemSelection & p_itemSelection ) override;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

	  private:
		// void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
