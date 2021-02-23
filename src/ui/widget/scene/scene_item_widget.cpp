#include "scene_item_widget.hpp"
#include "tool/logger.hpp"
#include "ui/mime_type.hpp"
#include "vtx_app.hpp"
#include <QDrag>

namespace VTX::UI::Widget::Scene
{
	SceneItemWidget::SceneItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void SceneItemWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setUniformRowHeights( true );
		setHeaderHidden( true );
		setColumnCount( 1 );
		setSelectionMode( QAbstractItemView::ExtendedSelection );
		setSelectionBehavior( SelectionBehavior::SelectRows );

		setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );

		setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
		setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAlwaysOff );
	}

	void SceneItemWidget::_setupSlots() {}

	void SceneItemWidget::localize() {}

	void SceneItemWidget::updatePosInSceneHierarchy( const int p_position ) { _refreshSize(); };
	void SceneItemWidget::mousePressEvent( QMouseEvent * p_event )
	{
		QTreeWidget::mousePressEvent( p_event );

		if ( p_event->button() == Qt::LeftButton )
			_dragStartPosition = p_event->pos();
	}

	void SceneItemWidget::dragEnterEvent( QDragEnterEvent * p_event )
	{
		BaseManualWidget::dragEnterEvent( p_event );
		if ( p_event->mimeData()->hasFormat(
				 VTX::UI::MimeType::getQStringMimeType( VTX::UI::MimeType::ApplicationMimeType::SCENE_ITEM ) ) )
			p_event->acceptProposedAction();
	}
	void SceneItemWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		BaseManualWidget::mouseMoveEvent( p_event );

		if ( !( p_event->buttons() & Qt::LeftButton ) )
			return;

		if ( !_canDragObjectAtPos( _dragStartPosition ) )
			return;

		if ( ( p_event->pos() - _dragStartPosition ).manhattanLength() < QApplication::startDragDistance() )
			return;

		QDrag *		drag	 = new QDrag( this );
		QMimeData * mimeData = _getDataForDrag();
		drag->setMimeData( mimeData );

		drag->exec( Qt::CopyAction | Qt::MoveAction );
	}

	void SceneItemWidget::_onItemExpanded( QTreeWidgetItem * const ) { _refreshSize(); }
	void SceneItemWidget::_onItemCollapsed( QTreeWidgetItem * const ) { _refreshSize(); }

	void SceneItemWidget::_refreshSize()
	{
		setMinimumHeight( _getMinimumHeight() );
		setMinimumWidth( sizeHintForColumn( 0 ) );
	}
	int SceneItemWidget::_getMinimumHeight() const
	{
		int nbItemDisplayed = 1;

		QModelIndex ptr		   = indexFromItem( topLevelItem( 0 ) );
		uint		childCount = 0;
		while ( ptr.isValid() && nbItemDisplayed < 4 )
		{
			const QModelIndex & child = ptr.model()->index( 0, 0, ptr );
			if ( isExpanded( ptr ) && child.isValid() )
			{
				ptr = child;
				nbItemDisplayed++;
				childCount = 0;
			}
			else
			{
				childCount++;

				ptr = ptr.model()->index( childCount, 0, ptr.parent() );

				if ( ptr.isValid() )
					nbItemDisplayed++;
			}
		}

		return nbItemDisplayed > 3 ? 0 : ( rowHeight( model()->index( 0, 0 ) ) * nbItemDisplayed );
	}

} // namespace VTX::UI::Widget::Scene
