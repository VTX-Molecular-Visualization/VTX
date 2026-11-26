#ifndef VTX_UI_QT_WIDGET_TREE_SCRIPTS_HPP
#define VTX_UI_QT_WIDGET_TREE_SCRIPTS_HPP

#include "ui/qt/events.hpp"
#include "ui/qt/widget/tree/base_tree.hpp"
#include <QFileSystemWatcher>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QTimer>
#include <QTreeWidget>
#include <util/event_hub.hpp>
#include <util/filesystem.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Tree reflecting the built-in example scripts and the user scripts on disk.
	 */
	class Scripts : public Widget::Tree::BaseTree<Scripts>
	{
	  public:
		/**
		 * @brief Item data role holding the path on disk.
		 */
		static constexpr int PATH_ROLE = Qt::UserRole;

		/**
		 * @brief Constructor.
		 */
		Scripts( QWidget * p_parent );

	  private:
		QTreeWidgetItem *			 _builtInItem = nullptr;
		QTreeWidgetItem *			 _userItem   = nullptr;
		QPointer<QFileSystemWatcher> _watcher;
		QPointer<QTimer>			 _refreshTimer;

		VTX::Util::EventHub::ScopedConnection _scriptCopiedConnection;

		/**
		 * @brief Rebuild the tree from disk, keeping expansion and selection.
		 */
		void _refresh();
		void _fill( QTreeWidgetItem &, const FilePath &, const QSet<QString> & );
		void _collectExpanded( const QTreeWidgetItem &, QSet<QString> & ) const;
		QTreeWidgetItem * _find( const QTreeWidgetItem &, const QString & ) const;
		QTreeWidgetItem * _find( const QString & ) const;
		void			  _select( const FilePath & );
		void			  _onScriptCopied( const Events::ScriptCopied & );
	};
} // namespace VTX::UI::QT::Widget::Tree

#endif
