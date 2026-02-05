#ifndef __VTX_UI_QT_DOCK_WIDGET_TREE_BASE_TREE__
#define __VTX_UI_QT_DOCK_WIDGET_TREE_BASE_TREE__

#include "ui/qt/widget/base_widget.hpp"
#include <QTreeWidget>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Base class for tree widgets that auto adjust their height to their content (only one top level item).
	 */
	template<typename T, ConceptWidget W = QTreeWidget>
	class BaseTree : public Widget::BaseWidget<T, W>
	{
	  public:
		/**
		 * @brief Default UI settings.
		 */
		BaseTree( QWidget * p_parent ) : Widget::BaseWidget<T, W>( p_parent )
		{
			W::setHeaderHidden( true );
			W::setItemsExpandable( true );
			W::setAllColumnsShowFocus( true );
			W::setUniformRowHeights( true );
			W::setExpandsOnDoubleClick( true );
			W::setEditTriggers( QAbstractItemView::NoEditTriggers );
			W::setFrameStyle( QFrame::NoFrame );
			W::setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum );
			W::setMinimumHeight( 0 );
			W::setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
			W::setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

			W::connect( this, &QTreeView::expanded, this, [ this ]( const QModelIndex & ) { W::updateGeometry(); } );
			W::connect( this, &QTreeView::collapsed, this, [ this ]( const QModelIndex & ) { W::updateGeometry(); } );
		}

		virtual ~BaseTree() = default;

		/**
		 * @brief Calculate the content height of the tree.
		 */
		int _contentHeight() const
		{
			const int rowHeight = this->sizeHintForRow( 0 );
			if ( rowHeight <= 0 )
			{
				return 0;
			}

			const QAbstractItemModel * const m = this->model();
			if ( m == nullptr )
			{
				return 0;
			}

			int rows = 0;

			const QModelIndex root = this->rootIndex();

			std::function<void( const QModelIndex & )> countRows = [ & ]( const QModelIndex & parent )
			{
				const int rc = m->rowCount( parent );
				for ( int r = 0; r < rc; ++r )
				{
					const QModelIndex idx = m->index( r, 0, parent );
					if ( not idx.isValid() )
					{
						continue;
					}

					++rows;

					if ( this->isExpanded( idx ) )
					{
						countRows( idx );
					}
				}
			};

			countRows( root );

			return rows * rowHeight + 2 * this->frameWidth();
		}

		/**
		 * @brief Override size hints to adapt height to content.
		 */
		inline QSize minimumSizeHint() const override { return QSize( 0, 0 ); }
		inline QSize sizeHint() const override { return QSize( QWidget::sizeHint().width(), _contentHeight() ); }
	};
} // namespace VTX::UI::QT::Widget::Tree

#endif
