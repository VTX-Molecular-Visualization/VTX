
#include "ui/qt/widget/tree/trajectory_slider.hpp"
#include <QPainter>
#include <QStyleOptionSlider>

namespace VTX::UI::QT::Widget::Tree
{

	TrajectorySlider::TrajectorySlider( Qt::Orientation p_orientation, QWidget * p_parent ) :
		QSlider( p_orientation, p_parent )
	{
	}

	void TrajectorySlider::setLoadedRange( int p_lower, int p_upper )
	{
		if ( _loadedLower != p_lower || _loadedUpper != p_upper )
		{
			_loadedLower = p_lower;
			_loadedUpper = p_upper;
			update();
		}
	}

	void TrajectorySlider::paintEvent( QPaintEvent * p_event )
	{
		// Draw the base slider first.
		QSlider::paintEvent( p_event );

		int range = maximum() - minimum();
		if ( range <= 0 || _loadedUpper <= _loadedLower )
		{
			return;
		}

		// Get groove geometry.
		QStyleOptionSlider opt;
		initStyleOption( &opt );
		QRect grooveRect = style()->subControlRect( QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this );

		// Map frame indices to pixel positions within the groove.
		double pixelPerUnit = static_cast<double>( grooveRect.width() ) / range;
		int	   xStart		= grooveRect.x() + static_cast<int>( ( _loadedLower - minimum() ) * pixelPerUnit );
		int	   xEnd			= grooveRect.x() + static_cast<int>( ( _loadedUpper - minimum() ) * pixelPerUnit );

		// Clamp to groove bounds.
		xStart = std::max( xStart, grooveRect.x() );
		xEnd   = std::min( xEnd, grooveRect.right() );

		// Paint the overlay.
		QRect overlay( xStart, grooveRect.y(), xEnd - xStart, grooveRect.height() );

		QPainter painter( this );
		painter.setClipRect( grooveRect );
		painter.fillRect( overlay, QColor( 80, 160, 255, 90 ) );
	}

} // namespace VTX::UI::QT::Widget::Tree
