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
	constexpr uint SEQ_CHAR_WIDTH  = 12;
	constexpr uint SEQ_CHAR_HEIGHT = 18;
	constexpr uint SEQ_RULE_STEP   = 5;

	Sequence::Sequence( const App::Component::Chemistry::System & p_system, QWidget * p_parent ) :

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

		const int	xOffset	   = horizontalScrollBar()->value();
		const Index startIndex = xOffset / SEQ_CHAR_WIDTH;
		Index		endIndex   = Util::Math::min(
			startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2, Index( _system.getResidues().size() )
		);

		int x = -( xOffset % int( SEQ_CHAR_WIDTH ) );

		// Label with current chain.
		const auto * firstResidue = _system.getResidue( startIndex );
		const auto * firstChain	  = firstResidue->getChainPtr();

		const QString headerLabel = QString( "%1/%2" ).arg(
			QString::fromStdString( _system.getName() ), QString::fromStdString( firstChain->getName() )
		);
		painter.setPen( Helper::toQColor( colorlayout.getPreset().getChainColor( firstChain->getIndex() + 1 ) ) );
		painter.drawText( 0, SEQ_CHAR_HEIGHT, headerLabel );
		const int labelWidth = painter.fontMetrics().horizontalAdvance( headerLabel );

		// Draw the residue sequence.
		const auto * lastChain = firstChain;
		for ( Index i = startIndex; i < endIndex; ++i )
		{
			const auto * residue = _system.getResidue( i );
			const auto * chain	 = residue->getChainPtr();
			painter.setPen(
				Helper::toQColor(
					colorlayout.getPreset().getChainColor( size_t( residue->getChainPtr()->getIndex() + 1 ) )
				)
			);

			// Chain labels.
			int labelChainWidth = 0;
			if ( chain != lastChain )
			{
				if ( x > labelWidth )
				{
					QString chainLabel = QString( "/%1" ).arg( QString::fromStdString( chain->getName() ) );
					painter.drawText( x, SEQ_CHAR_HEIGHT, chainLabel );
					labelChainWidth = painter.fontMetrics().horizontalAdvance( chainLabel );
				}
				lastChain = chain;
			}

			// Rule.
			const size_t indexInChain = i - chain->getIndexFirstResidue() + 1;
			if ( x > labelWidth && x > labelChainWidth && indexInChain % SEQ_RULE_STEP == 0 )
			{
				painter.drawText( x, SEQ_CHAR_HEIGHT, QString::number( indexInChain ) );
			}

			// Residue symbol.
			// painter.setPen( Helper::toQColor( colorlayout.getResidueColor( size_t( residue->getSymbol() ) ) ) );
			painter.drawText( x, SEQ_CHAR_HEIGHT * 2, QString( residue->getShortName().at( 0 ) ) );

			x += SEQ_CHAR_WIDTH;
		}
	}

	void Sequence::mousePressEvent( QMouseEvent * p_event )
	{
		const int	 xOffset   = horizontalScrollBar()->value();
		const int	 yOffset   = verticalScrollBar()->value();
		const int	 clickX	   = p_event->pos().x() + xOffset;
		const int	 clickY	   = p_event->pos().y() + yOffset;
		const size_t index	   = clickX / SEQ_CHAR_WIDTH;
		const int	 topMargin = contentsMargins().top();

		if ( ( clickY > ( topMargin + int( SEQ_CHAR_HEIGHT ) ) )
			 && ( clickY < ( topMargin + int( SEQ_CHAR_HEIGHT ) * 2 ) ) && ( index >= 0 )
			 && ( index < _system.getResidues().size() ) )
		{
			qDebug() << "Residue clicked:" << index;
		}
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { updateScrollBars(); }

	void Sequence::updateScrollBars()
	{
		const uint contentWidth = uint( _system.getResidues().size() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}
} // namespace VTX::UI::QT::Widget
