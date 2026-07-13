#ifndef __VTX_UI_QT_WIDGET_SEQUENCE__
#define __VTX_UI_QT_WIDGET_SEQUENCE__

#include <QAbstractScrollArea>
#include <QContextMenuEvent>
#include <app/ecs.hpp>
#include <core/struct/topology.hpp>
#include <optional>
#include <ui/qt/events.hpp>
#include <unordered_map>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom widget for sequence that paint only the displayed area.
	 */
	class Sequence : public QAbstractScrollArea
	{
	  public:
		Sequence( const Entity, QWidget * );

		enum struct Mode : int
		{
			OriginalResId	= 0,
			contiguousResId = 1,
		};

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
		 * @brief Sequence viewer are able to display the author res id. Since this number is usually higher than the
		 * number of residues, we need to hold that value to compute the window width
		 */
		Index _lastResidueOriginalIndex;

		/**
		 * @brief When the sequence viewer displays the author resid, it needs a table to quickly access the vtx resid
		 * during paint
		 */
		std::unordered_map<Index, Index> _originalIndex2VtxIndexMapping;

		Util::EventHub::ScopedConnection _changeModeConnection;

		/**
		 * @brief UI controls.
		 */
		Index _anchor		  = INVALID_INDEX;
		Index _lastClicked	  = INVALID_INDEX;
		bool  _dragging		  = false;
		bool  _dragAddMode	  = false;
		Index _dragStartIndex = INVALID_INDEX;

		Index _lastResidueIndex() const;

		/**
		 * @brief During the paint event, we can use either the author resid or the contiguous resid. Either way, the
		 * data we fetch from the topology uses the contiguous resid. Therefore, we need a way to get the contiguous
		 * resid regardless of the currently used resid.
		 * @return
		 */
		std::function<std::optional<Index>( const Index & )> _residueIndexConverter() const;

		/**
		 * @brief returns a getter to a chain index from the screen Index, regardless of the mode.
		 * @return
		 */
		std::function<Index( const Core::Struct::Topology &, const Index & )> _residueChainResolver() const;

		void _onModeChanged( const Events::SequenceResIdChanged & );
		void _updateScrollBars();

		/**
		 * @brief Get residue index from position in viewport.
		 */
		std::optional<Index> _indexFromPos( const QPoint & );
	};

} // namespace VTX::UI::QT::Widget

#endif
