#include "ui/qt/widget/sequence.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <app/ecs.hpp>
#include <app/library/preset/color_layout.hpp>
#include <app/system/metadata.hpp>
#include <app/system/uid.hpp>
#include <core/chemdb/residue.hpp>
#include <core/struct/system.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{
	constexpr uint SEQ_CHAR_WIDTH  = 12;
	constexpr uint SEQ_CHAR_HEIGHT = 18;
	constexpr uint SEQ_RULE_STEP   = 5;

	Sequence::Sequence( const App::ECS::Entity p_system, QWidget * p_parent ) :

		QAbstractScrollArea( p_parent ), _system( p_system )
	{
		setFont( QFont( "Courier", 10 ) );
		setMouseTracking( true );
	}

	void Sequence::paintEvent( QPaintEvent * p_event )
	{
		QPainter painter( viewport() );
		painter.setFont( font() );

		using namespace App;

		auto & system	   = REG().get<Core::Struct::System>( _system );
		auto & metadata	   = REG().get<System::Metadata>( _system );
		auto & uid		   = REG().get<System::UID>( _system );
		auto & colorlayout = ECS::getFirstComponent<Library::Preset::ColorLayout>();
		auto & selection   = QT::SELECTION();

		const int	xOffset	   = horizontalScrollBar()->value();
		const Index startIndex = xOffset / SEQ_CHAR_WIDTH;

		Index endIndex
			= Util::Math::min( startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2, system.getResidueCount() );

		if ( endIndex <= startIndex )
		{
			return;
		}

		int x = -( xOffset % int( SEQ_CHAR_WIDTH ) );

		// Label with current chain.
		const Index firstChain = system.residueChainIndexes[ startIndex ];

		const QString headerLabel = QString( "%1/%2" ).arg(
			QString::fromStdString( metadata.pdbIDCode ), QString::fromStdString( system.chainNames[ firstChain ] )
		);
		painter.setPen( Helper::toQColor( colorlayout.getChainColor( firstChain + 1 ) ) );
		painter.drawText( 0, SEQ_CHAR_HEIGHT, headerLabel );
		const int labelWidth = painter.fontMetrics().horizontalAdvance( headerLabel );

		// Draw the residue sequence.
		Index lastChain = firstChain;
		for ( Index residue = startIndex; residue < endIndex; ++residue )
		{
			const Index chain = system.residueChainIndexes[ residue ];
			painter.setPen( Helper::toQColor( colorlayout.getChainColor( size_t( chain + 1 ) ) ) );

			// Chain labels.
			int labelChainWidth = 0;
			if ( chain != lastChain )
			{
				if ( x > labelWidth )
				{
					QString chainLabel = QString( "/%1" ).arg( QString::fromStdString( system.chainNames[ chain ] ) );
					painter.drawText( x, SEQ_CHAR_HEIGHT, chainLabel );
					labelChainWidth = painter.fontMetrics().horizontalAdvance( chainLabel );
				}
				lastChain = chain;
			}

			// Rule.
			const size_t indexInChain = residue - system.chainFirstResidues[ chain ] + 1;
			if ( x > labelWidth && x > labelChainWidth && indexInChain % SEQ_RULE_STEP == 0 )
			{
				painter.drawText( x, SEQ_CHAR_HEIGHT, QString::number( indexInChain ) );
			}

			// TODO
			// Selection.
			const QRect cellRect( x, SEQ_CHAR_HEIGHT + 5, SEQ_CHAR_WIDTH, SEQ_CHAR_HEIGHT );
			bool		selected = false;
			if ( selected )
			{
				painter.fillRect( cellRect, palette().highlight() );
			}

			// Residue symbol.
			// Display residue standardized symbol or name if unknown.
			const auto symbol = system.residueSymbols[ residue ];
			const auto name	  = symbol != Core::ChemDB::Residue::SYMBOL::UNKNOWN
									? Core::ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ]
									: system.residueNames[ residue ];
			painter.drawText( x, SEQ_CHAR_HEIGHT * 2, QString( name.at( 0 ) ) );

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

		/*
		if ( ( clickY > ( topMargin + int( SEQ_CHAR_HEIGHT ) ) )
			 && ( clickY < ( topMargin + int( SEQ_CHAR_HEIGHT ) * 2 ) ) && ( index >= 0 )
			 && ( index < _system.getResidues().size() ) )
		{
			qDebug() << "Residue clicked:" << index;
		}
		*/
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { updateScrollBars(); }

	void Sequence::updateScrollBars()
	{
		auto &	   system		= App::REG().get<Core::Struct::System>( _system );
		const uint contentWidth = uint( system.getResidueCount() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}
} // namespace VTX::UI::QT::Widget
