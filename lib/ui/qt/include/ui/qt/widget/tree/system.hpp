#ifndef __VTX_UI_QT_WIDGET_TREE_SYSTEM__
#define __VTX_UI_QT_WIDGET_TREE_SYSTEM__

#include "ui/qt/model/system_model.hpp"
#include "ui/qt/widget/tree/base_tree.hpp"
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPersistentModelIndex>
#include <QTreeView>
#include <QVBoxLayout>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Scene tree
	 */
	class System : public Widget::Tree::BaseTree<System, QTreeView>
	{
		Q_OBJECT

	  public:
		System( const Entity, QWidget * );
		void							  contextMenuEvent( QContextMenuEvent * p_e ) override;
		void							  mousePressEvent( QMouseEvent * p_e ) override;
		void							  mouseMoveEvent( QMouseEvent * p_e ) override;
		void							  mouseReleaseEvent( QMouseEvent * p_e ) override;
		inline const Model::SystemModel & getSystemModel() { return *static_cast<Model::SystemModel *>( model() ); }
		inline const Model::SystemModel & getSystemModel() const
		{
			return *static_cast<Model::SystemModel *>( model() );
		}

	  private:
		/**
		 * @brief Entity to display.
		 */
		const Entity _system;

		/**
		 * @brief Layout.
		 */
		QVBoxLayout * _layout;

		/**
		 * @brief Selection anchor used for shift selection.
		 */
		QPersistentModelIndex _anchor;

		/**
		 * @brief Current drag state.
		 */
		bool _dragging = false;

		/**
		 * @brief Last dragged index to avoid duplicate append on the same row.
		 */
		QPersistentModelIndex _lastDraggedIndex;

		bool _shouldHandleSelectionClick( QMouseEvent *, const QModelIndex &, QStyleOptionViewItem & ) const;
		bool _isFullySelected( const QModelIndex & ) const;
		void _applySelection( const QModelIndex &, const bool = true, const bool = false );
		void _selectVisibleRange( const QModelIndex &, const QModelIndex & );
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
