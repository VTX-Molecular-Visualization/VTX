#include "ui/qt/widget/sequence.hpp"
#include "ui/qt/helper.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{

	Sequence::Sequence( App::Component::Chemistry::System & p_system, QWidget * p_parent ) :

		QAbstractScrollArea( p_parent ), _system( p_system )
	{
		setFont( QFont( "Courier", 10 ) );
		setMouseTracking( true );
	}

	void Sequence::paintEvent( QPaintEvent * p_event )
	{
		QPainter painter( viewport() );
		painter.setFont( font() );

		const auto & scene		 = App::SCENE();
		const auto & colorlayout = App::ECS_REGISTRY().getComponent<App::Component::Representation::ColorLayout>(
			App::ECS_REGISTRY().getEntity( scene )
		);

		int	 xOffset	= horizontalScrollBar()->value();
		int	 startIndex = xOffset / DEFAULT_CHAR_WIDTH;
		uint endIndex	= Util::Math::min(
			  startIndex + ( viewport()->width() / DEFAULT_CHAR_WIDTH ) + 2, uint( _system.getResidues().size() )
		  );

		int x = -( xOffset % int( DEFAULT_CHAR_WIDTH ) );

		for ( size_t i = startIndex; i < endIndex; ++i )
		{
			const auto * residue = _system.getResidue( i );
			painter.setPen( Helper::toQColor( colorlayout.getResidueColor( size_t( residue->getSymbol() ) ) ) );
			painter.drawText( x, DEFAULT_CHAR_HEIGHT, QString( residue->getShortName().at( 0 ) ) );
			x += DEFAULT_CHAR_WIDTH;
		}
	}

	void Sequence::mousePressEvent( QMouseEvent * p_event )
	{
		int	   xOffset = horizontalScrollBar()->value();
		int	   clickX  = p_event->pos().x() + xOffset;
		size_t index   = clickX / DEFAULT_CHAR_WIDTH;

		if ( index >= 0 && index < _system.getResidues().size() )
		{
			qDebug() << "Residue clicked:" << index;
		}
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { updateScrollBars(); }

	void Sequence::updateScrollBars()
	{
		const uint contentWidth = uint( _system.getResidues().size() ) * DEFAULT_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}
} // namespace VTX::UI::QT::Widget
