#ifndef __VTX_VIEW_UI_WIDGET_MEASUREMENT_DIHEDRAL_ANGLE_SCENE__
#define __VTX_VIEW_UI_WIDGET_MEASUREMENT_DIHEDRAL_ANGLE_SCENE__

#include "tool/old_tool/model/measurement/dihedral_angle.hpp"
#include <QMimeData>
#include <QWidget>
#include <app/old/id.hpp>
#include <app/old/core/view/base_view.hpp>
#include <string>
#include <ui/old_ui/ui/widget/scene/scene_item_widget.hpp>

namespace VTX::View::UI::Widget::Measurement
{
	class DihedralAngleSceneView :
		public App::Old::Core::View::BaseView<Model::Measurement::DihedralAngle>,
		public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_MODEL

	  public:
		virtual const App::Old::Core::Model::ID &			   getModelID() const { return _model->getId(); };
		virtual const App::Old::Core::Scene::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		void notify( const App::Old::Core::Event::VTXEvent * const p_event ) override;

	  protected:
		DihedralAngleSceneView( Model::Measurement::DihedralAngle * const p_model, QWidget * const p_parent );
		void _setupSlots() override;

		void _fillItemSelection( const App::Old::Application::Selection::SelectionModel & p_selection, QItemSelection & p_itemSelection ) override;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

	  private:
		// void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;
	};

} // namespace VTX::View::UI::Widget::Measurement
#endif
