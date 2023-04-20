#ifndef __VTX_VIEW_UI_WIDGET_PATH_SCENE_VIEW__
#define __VTX_VIEW_UI_WIDGET_PATH_SCENE_VIEW__

#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/scene/scene_item_widget.hpp"
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QString>
#include <QTreeWidgetItem>
#include <QWidget>
#include <app/event/base_event_receiver_vtx.hpp>
#include <app/old_app/generic/base_visible.hpp>
#include <app/old_app/id.hpp>
#include <app/old_app/model/path.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/old_app/model/viewpoint.hpp>
#include <app/view/base_view.hpp>
#include <map>

namespace VTX::View::UI::Widget
{
	class PathSceneView : public View::BaseView<Model::Path>, public VTX::UI::Widget::Scene::SceneItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;
		void notify( const VTX::Event::VTXEvent * const p_event ) override;
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

		const Model::ID &			   getModelID() const override { return _model->getId(); };
		const Generic::BaseSceneItem & getBaseSceneItem() const { return *_model; };
		virtual bool				   containsModel( const Model::BaseModel & p_model ) const override;
		virtual std::vector<Model::ID> getAllItemsFrom( const Model::BaseModel & p_model ) const override;
		virtual std::vector<Model::ID> getAllItemsTo( const Model::BaseModel & p_model ) const override;

		QTreeWidgetItem * getLastVisibleItem() override;

	  protected:
		PathSceneView( Model::Path * const p_model, QWidget * const p_parent );
		~PathSceneView();

		void _setupUi( const QString & ) override;
		void _setupSlots() override;

		void _onCustomContextMenuCalled( const QPoint & p_clicPos ) override;
		void _onItemExpanded( QTreeWidgetItem * const ) override;

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
