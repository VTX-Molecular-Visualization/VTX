#include "scene_item_widget.hpp"
#include "ui/mime_type.hpp"
#include "vtx_app.hpp"
#include <QDrag>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
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

					setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
					_contextMenu = new QMenu( parentWidget() );

					setAcceptDrops( true );
				}

				void SceneItemWidget::_setupSlots()
				{
					connect( this,
							 &QTreeWidget::customContextMenuRequested,
							 this,
							 &SceneItemWidget::_onCustomContextMenuCalled );
					_contextMenu->addAction( "Delete", this, &SceneItemWidget::_deleteAction, QKeySequence::Delete );
				}

				void SceneItemWidget::localize() {}

				void SceneItemWidget::_onCustomContextMenuCalled( const QPoint & p_clicPos )
				{
					_contextMenu->popup( mapToGlobal( p_clicPos ) );
				}

				void SceneItemWidget::mousePressEvent( QMouseEvent * p_event )
				{
					QTreeWidget::mousePressEvent( p_event );

					if ( p_event->button() == Qt::LeftButton )
						_dragStartPosition = p_event->pos();
				}

				void SceneItemWidget::dragEnterEvent( QDragEnterEvent * p_event )
				{
					BaseManualWidget::dragEnterEvent( p_event );
					if ( p_event->mimeData()->hasFormat( VTX::UI::MimeType::getQStringMimeType(
							 VTX::UI::MimeType::ApplicationMimeType::SCENE_ITEM ) ) )
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
			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
