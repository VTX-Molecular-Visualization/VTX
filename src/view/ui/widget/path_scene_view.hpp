#ifndef __VTX_VIEW_UI_WIDGET_PATH_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_PATH_SCENE_VIEW__

#include "event/base_event_receiver_vtx.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "model/path.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "style.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/scene/scene_item_widget.hpp"
#include "view/base_view.hpp"
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>
#include <map>

namespace VTX::View::UI::Widget
{
	class PathSceneView : public View::BaseView<Model::Path>, public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;
		void notify( const Event::VTXEvent * const p_event ) override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		const Model::ID &			   getModelID() const override { return _model->getId(); };
		const Generic::BaseSceneItem & getBaseSceneItem() const { return *_model; };

		QTreeWidgetItem * getLastVisibleItem() override;

	  protected:
		PathSceneView( Model::Path * const p_model, QWidget * const p_parent );
		~PathSceneView();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;

		void _selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append = false );

		void _createTopLevelObject() override;
		void _fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection ) override;

	  private:
		QMenu *					_contextMenu;
		const QTreeWidgetItem * _lastItemClicked = nullptr;
		QTreeWidgetItem *		_lastItemVisible = nullptr;

		void _onItemChanged( QTreeWidgetItem * const, const int );
		void _onItemDoubleClicked( const QTreeWidgetItem * const, const int ) const;

		QTreeWidgetItem * _itemFromViewpoint( const Model::Viewpoint & p_viewpoint ) const;
		void			  _addViewpoint( const Model::Viewpoint * const p_viewpoint );
		void			  _removeViewpoint( const Model::Viewpoint * const p_viewpoint );

		void _reformatName( std::string & p_name ) const;
	};

} // namespace VTX::View::UI::Widget

#endif
