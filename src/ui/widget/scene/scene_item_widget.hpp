#ifndef __VTX_UI_WIDGET_SCENE_ITEM__
#define __VTX_UI_WIDGET_SCENE_ITEM__

#include "generic/base_scene_item.hpp"
#include "model/base_model.hpp"
#include "ui/draggable_item.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QTreeWidget>

namespace VTX::Generic
{
	class BaseVisible;
};
namespace VTX::Model
{
	class Selection;
};

namespace VTX::UI::Widget::Scene
{
	class SceneItemWidget : public BaseManualWidget<QTreeWidget>, public DraggableItem
	{
		VTX_WIDGET

	  public:
		inline static const Qt::ItemDataRole MODEL_ID_ROLE	   = Qt::ItemDataRole( Qt::UserRole );
		inline static const Qt::ItemDataRole EXPAND_STATE_ROLE = Qt::ItemDataRole( Qt::UserRole + 1 );

	  public:
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		virtual void updatePosInSceneHierarchy( const int p_position );

		virtual const Model::ID &			   getModelID() const		= 0;
		virtual const Generic::BaseSceneItem & getBaseSceneItem() const = 0;
		virtual QTreeWidgetItem *			   getLastVisibleItem();

		void openRenameEditor( const Model::ID & p_modelID );

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

		void		 _refreshSelection( const Model::Selection & p_selection );
		virtual void _fillItemSelection( const Model::Selection & p_selection, QItemSelection & p_itemSelection );

		virtual void _refreshSize();
		virtual int	 _getMinimumHeight() const;

		void _refreshItemVisibility( QTreeWidgetItem * const p_itemWidget, const bool p_visible );
		void _refreshItemsVisibility();
		void _refreshItemsVisibilityRecursive( QTreeWidgetItem & p_parent );

		void _enableSignals( const bool p_enable );

		void _selectItemWithArrows( QTreeWidgetItem & p_itemToSelect, const bool p_append = false );

		virtual bool _itemCanBeRenamed( const QTreeWidgetItem * p_item );

		void _refreshCurrentItemInSelection( const Model::BaseModel * const p_obj );

		Model::ID				  _getModelIDFromItem( const QTreeWidgetItem & p_item ) const;
		virtual QTreeWidgetItem * _findItemFromModelID( const Model::ID & p_id ) const;
		QTreeWidgetItem * _findItemFromModelIDRecursive( QTreeWidgetItem & p_parent, const Model::ID & p_id ) const;
		bool			  _getItemExpandState( const QTreeWidgetItem & p_item ) const;

	  private:
		int _enableSignalCounter = 0;
	};

} // namespace VTX::UI::Widget::Scene

#endif
