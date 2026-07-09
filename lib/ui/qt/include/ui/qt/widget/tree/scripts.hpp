#ifndef __VTX_UI_QT_WIDGET_TREE_SCRIPTS__
#define __VTX_UI_QT_WIDGET_TREE_SCRIPTS__

#include "ui/qt/widget/tree/base_tree.hpp"
#include <QFileSystemModel>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QTreeView>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Tree reflecting the content of the user scripts directory.
	 */
	class Scripts : public Widget::Tree::BaseTree<Scripts, QTreeView>
	{
	  public:
		Scripts( QWidget * const p_parent );

	  private:
		QPointer<QFileSystemModel>		_fileModel;
		QPointer<QSortFilterProxyModel> _proxyModel;

		void _selectScript( const QModelIndex & p_index );
		void _runScript( const QModelIndex & p_index );
	};
} // namespace VTX::UI::QT::Widget::Tree

#endif
