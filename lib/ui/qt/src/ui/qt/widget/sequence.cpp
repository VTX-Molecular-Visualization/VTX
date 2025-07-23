#include "ui/qt/widget/sequence.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{

	Sequence::Sequence( QWidget * p_parent ) : QAbstractScrollArea( p_parent )
	{
		setFont( QFont( "Courier", 10 ) );
		setMouseTracking( true );
	}

	void Sequence::setSequence( const QVector<QChar> & p_sequence, const QVector<QColor> & p_colors )
	{
		_sequence = p_sequence;
		_colors	  = p_colors;
		updateScrollBars();
		viewport()->update();
	}

	void Sequence::paintEvent( QPaintEvent * p_event )
	{
		QPainter painter( viewport() );
		painter.setFont( font() );

		int	 xOffset	= horizontalScrollBar()->value();
		int	 startIndex = xOffset / DEFAULT_CHAR_WIDTH;
		uint endIndex	= Util::Math::min(
			  startIndex + ( viewport()->width() / DEFAULT_CHAR_WIDTH ) + 2, uint( _sequence.size() )
		  );

		int x = -( xOffset % int( DEFAULT_CHAR_WIDTH ) );

		for ( size_t i = startIndex; i < endIndex; ++i )
		{
			painter.setPen( _colors[ i ] );
			painter.drawText( x, DEFAULT_CHAR_HEIGHT, QString( _sequence[ i ] ) );
			x += DEFAULT_CHAR_WIDTH;
		}
	}

	void Sequence::mousePressEvent( QMouseEvent * p_event )
	{
		int xOffset = horizontalScrollBar()->value();
		int clickX	= p_event->pos().x() + xOffset;
		int index	= clickX / DEFAULT_CHAR_WIDTH;

		if ( index >= 0 && index < _sequence.size() )
		{
			emit residueClicked( index );
		}
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { updateScrollBars(); }

	void Sequence::updateScrollBars()
	{
		int contentWidth = _sequence.size() * DEFAULT_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}
} // namespace VTX::UI::QT::Widget
