#ifndef __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM__
#define __VTX_UI_QT_TOOL_SCENE_WIDGET_SCENE_ITEM__

#include "ui/qt/draggable_item.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QTreeWidget>
#include <app/application/selection/_fwd.hpp>
#include <app/core/model/base_model.hpp>
#include <app/core/scene/base_scene_item.hpp>
#include <vector>

namespace VTX::Generic
{
	class BaseVisible;
};

namespace VTX::UI::QT::Tool::Scene::Widget
{
	class SceneItemWidget : public QT::Widget::BaseManualWidget<QTreeWidget>, public DraggableItem
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		inline static const Qt::ItemDataRole MODEL_ID_ROLE	   = Qt::ItemDataRole( Qt::UserRole );
		inline static const Qt::ItemDataRole EXPAND_STATE_ROLE = Qt::ItemDataRole( Qt::UserRole + 1 );

	  protected:
		inline static const Qt::ItemDataRole CHILD_FIRST_ROLE = Qt::ItemDataRole( Qt::UserRole + 2 );

	  public:
		void localize() override;
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;

		virtual void updatePosInSceneHierarchy( const int p_position );

		virtual const App::Core::Model::ID &			getModelID() const		 = 0;
		virtual const App::Core::Scene::BaseSceneItem & getBaseSceneItem() const = 0;
		virtual QTreeWidgetItem *						getLastVisibleItem();

		virtual bool containsModel( const App::Core::Model::BaseModel & p_model ) const
		{
			return p_model.getId() == getModelID();
		}
		virtual std::vector<App::Core::Model::ID> getAllItemsFrom( const App::Core::Model::BaseModel & p_model ) const;
		virtual std::vector<App::Core::Model::ID> getAllItemsTo( const App::Core::Model::BaseModel & p_model ) const;

		void openRenameEditor( const App::Core::Model::ID & p_modelID );

	  protected:
		SceneItemWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		virtual void keyPressEvent( QKeyEvent * p_event ) override;

		virtual void mousePressEvent( QMouseEvent * p_event ) override;
		virtual void mouseMoveEvent( QMouseEvent * p_event ) override;
		void		 dragEnterEvent( QDragEnterEvent * p_event ) override;

		virtual void _onItemExpanded( QTreeWidgetItem * const );
		virtual void _onItemCollapsed( QTreeWidgetItem * const );
		virtual void _onCustomContextMenuCalled( const QPoint & p_clicPos );

		virtual void _createTopLevelObject();
		void		 _openRenameEditor( QTreeWidgetItem & p_target );

		void		 _refreshSelection( const App::Application::Selection::SelectionModel & p_selection );
		virtual void _fillItemSelection( const App::Application::Selection::SelectionModel & p_selection,
										 QItemSelection &								p_itemSelection );

		virtual void _refreshSize();
		virtual int	 _getMinimumHeight() const;

		void _refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const bool p_visible );
		void _refreshItemsVisibility();
		void _refreshItemsVisibilityRecursive( QTreeWidgetItem & p_parent );

		void _enableSignals( const bool p_enable );

		void _selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append = false );

		virtual bool _itemCanBeRenamed( const QTreeWidgetItem * p_item );

		void _refreshCurrentItemInSelection( const App::Core::Model::BaseModel * const p_obj );

		App::Core::Model::ID	  _getModelIDFromItem( const QTreeWidgetItem & p_item ) const;
		virtual QTreeWidgetItem * _findItemFromModelID( const App::Core::Model::ID & p_id ) const;
		QTreeWidgetItem *		  _findItemFromModelIDRecursive( QTreeWidgetItem &			  p_parent,
																 const App::Core::Model::ID & p_id ) const;
		bool					  _getItemExpandState( const QTreeWidgetItem & p_item ) const;

		QMimeData * _getDataForDrag() const override;

	  private:
		int _enableSignalCounter = 0;
	};

} // namespace VTX::UI::QT::Tool::Scene::Widget

#endif
