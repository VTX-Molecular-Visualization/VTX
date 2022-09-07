#include "draggable_item.hpp"
#include <QApplication>
#include <QDrag>

namespace VTX::UI
{
	DraggableItem::DragActionCatcher::DragActionCatcher( DraggableItem * const p_owner, QWidget * const p_target ) :
		_owner( p_owner )
	{
		p_target->installEventFilter( this );
		p_target->setMouseTracking( true );
	}

	bool DraggableItem::DragActionCatcher::eventFilter( QObject * p_watched, QEvent * p_event )
	{
		bool res = false;
		if ( p_event->type() == QEvent::Type::MouseButtonPress )
		{
			_checkMousePressEvent( dynamic_cast<QMouseEvent *>( p_event ) );
			res = true;
		}
		else if ( p_event->type() == QEvent::Type::MouseMove )
		{
			_checkMouseMoveEvent( dynamic_cast<QMouseEvent *>( p_event ) );
			res = true;
		}

		return res;
	}

	void DraggableItem::DragActionCatcher::_checkMousePressEvent( QMouseEvent * p_event )
	{
		if ( p_event->button() == Qt::LeftButton )
			_dragStartPosition = p_event->pos();

		p_event->accept();
	}
	void DraggableItem::DragActionCatcher::_checkMouseMoveEvent( QMouseEvent * p_event )
	{
		if ( !( p_event->buttons() & Qt::LeftButton ) )
			return;

		if ( !_owner->_canDragAtPos( _dragStartPosition ) )
			return;

		if ( ( p_event->pos() - _dragStartPosition ).manhattanLength() < QApplication::startDragDistance() )
			return;

		QMimeData * const mimeData = _owner->_getDataForDrag();

		if ( mimeData == nullptr )
			return;

		QDrag * const drag = new QDrag( this );
		drag->setMimeData( mimeData );
		drag->exec( Qt::CopyAction | Qt::MoveAction );
	}

	DraggableItem::DraggableItem( QWidget * const p_item ) : _dragActionCatcher( this, p_item ) {}

} // namespace VTX::UI
