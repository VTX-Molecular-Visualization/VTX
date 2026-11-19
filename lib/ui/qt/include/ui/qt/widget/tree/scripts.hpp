#ifndef VTX_UI_QT_WIDGET_TREE_SCRIPTS_HPP
#define VTX_UI_QT_WIDGET_TREE_SCRIPTS_HPP

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
		/**
		 * @brief Constructor.
		 */
		Scripts( QWidget * p_parent );

	  protected:
		/**
		 * @brief Override of the selection command to run the script on double click.
		 */
		QItemSelectionModel::SelectionFlags selectionCommand( const QModelIndex &, const QEvent * ) const override;

	  private:
		/**
		 * @brief File system model.
		 */
		QPointer<QFileSystemModel> _fileModel;

		/**
		 * @brief Proxy model to filter the file system model.
		 */
		QPointer<QSortFilterProxyModel> _proxyModel;

		void _runScript( const QModelIndex & p_index );
	};
} // namespace VTX::UI::QT::Widget::Tree

#endif
