#ifndef __VTX_UI_QT_WIDGET_SEQUENCE__
#define __VTX_UI_QT_WIDGET_SEQUENCE__

#include <QAbstractScrollArea>
#include <QContextMenuEvent>
#include <app/ecs.hpp>
#include <core/struct/topology.hpp>
#include <optional>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom widget for sequence that paint only the displayed area.
	 */
	class Sequence : public QAbstractScrollArea
	{
	  public:
		Sequence( const Entity, QWidget * );

	  protected:
		void paintEvent( QPaintEvent * ) override;
		void contextMenuEvent( QContextMenuEvent * ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * e ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void resizeEvent( QResizeEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * ) override;

	  private:
		/**
		 * @brief Entity of the system to display.
		 */
		const Entity _system;

		/**
		 * @brief UI controls.
		 */
		Index _anchor		  = INVALID_INDEX;
		Index _lastClicked	  = INVALID_INDEX;
		bool  _dragging		  = false;
		bool  _dragAddMode	  = false;
		Index _dragStartIndex = INVALID_INDEX;

		void _updateScrollBars();

		/**
		 * @brief Get residue index from position in viewport.
		 */
		std::optional<Index> _indexFromPos( const QPoint & );
	};

} // namespace VTX::UI::QT::Widget

#endif
