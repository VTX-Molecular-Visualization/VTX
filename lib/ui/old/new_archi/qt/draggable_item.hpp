#ifndef __VTX_UI_QT_DRAGGABLE_ITEM__
#define __VTX_UI_QT_DRAGGABLE_ITEM__

#include <QMimeData>
#include <QMouseEvent>
#include <QObject>
#include <QPoint>
#include <QWidget>

namespace VTX::UI::QT
{
	class DraggableItem
	{
	  private:
		class DragActionCatcher : public QObject
		{
		  public:
			DragActionCatcher( DraggableItem * const p_owner, QWidget * const p_target );
			virtual bool eventFilter( QObject * p_watched, QEvent * p_event ) override;

		  protected:
			void _checkMousePressEvent( QMouseEvent * p_event );
			void _checkMouseMoveEvent( QMouseEvent * p_event );

		  private:
			DraggableItem * const _owner;
			QPoint				  _dragStartPosition = QPoint( 0, 0 );
		};

	  protected:
		DraggableItem( QWidget * const p_item );

		virtual bool		_canDragAtPos( const QPoint & p_position ) const { return true; }
		virtual QMimeData * _getDataForDrag() const = 0;

	  private:
		const DragActionCatcher _dragActionCatcher;
	};

} // namespace VTX::UI::QT

#endif
