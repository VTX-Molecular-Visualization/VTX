#include "ui/qt/widget/sequence.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <app/action/selection.hpp>
#include <app/ecs.hpp>
#include <app/helper/system.hpp>
#include <app/library/preset/color_layout.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <app/system/uid.hpp>
#include <app/system/visibility.hpp>
#include <core/chemdb/residue.hpp>
#include <core/struct/system.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{
	constexpr int SEQ_CHAR_WIDTH  = 12;
	constexpr int SEQ_CHAR_HEIGHT = 18;
	constexpr int SEQ_RULE_STEP	  = 5;

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

		auto & reg		   = REG();
		auto & system	   = reg.get<Core::Struct::System>( _system );
		auto & metadata	   = reg.get<System::Metadata>( _system );
		auto & uid		   = reg.get<System::UID>( _system );
		auto & colorlayout = ECS::getFirstComponent<Library::Preset::ColorLayout>();
		auto & selection   = reg.get<System::Selection>( _system );

		const int	xOffset	   = horizontalScrollBar()->value();
		const Index startIndex = xOffset / SEQ_CHAR_WIDTH;

		Index endIndex
			= Util::Math::min( startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2, system.getResidueCount() );

		if ( endIndex <= startIndex )
		{
			return;
		}

		int x = -( xOffset % SEQ_CHAR_WIDTH );

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

			// Selection.
			const QRect cellRect( x - 2, SEQ_CHAR_HEIGHT + 5, SEQ_CHAR_WIDTH, SEQ_CHAR_HEIGHT );
			bool		selected = App::Helper::System::isSelected<Scene::E_ITEM::RESIDUE>( _system, residue );
			if ( selected )
			{
				painter.fillRect( cellRect, palette().highlight() );
			}
			else
			{
				painter.fillRect( cellRect, palette().base() );
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

	void Sequence::mousePressEvent( QMouseEvent * p_e )
	{
		using namespace App;
		using namespace App::Action;

		auto opt = _indexFromPos( p_e->pos() );
		if ( not opt )
		{
			return;
		}

		Index index = *opt;

		auto & reg		 = REG();
		auto & selection = reg.get<System::Selection>( _system );

		bool shift = p_e->modifiers() & Qt::ShiftModifier;
		bool ctrl  = p_e->modifiers() & Qt::ControlModifier;

		bool selected = App::Helper::System::isSelected<Scene::E_ITEM::RESIDUE>( _system, index );

		if ( not shift && not ctrl )
		{
			// Normal.
			ACTION().execute<Selection::Clear>( _system );
			ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>( _system, index );

			_anchor		 = index;
			_lastClicked = index;
		}
		else if ( shift )
		{
			// SHIFT.
			if ( _anchor == INVALID_INDEX )
			{
				_anchor = index;
			}

			Index a = Util::Math::min( _anchor, index );
			Index b = Util::Math::max( _anchor, index );

			ACTION().execute<Selection::Clear>( _system );
			ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>(
				_system, Core::Struct::IndexRange { a, b }
			);

			_lastClicked = index;
		}
		else if ( ctrl )
		{
			// CTRL
			if ( selected )
			{
				ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>( _system, index, false );
			}
			else
			{
				ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>( _system, index );
			}

			_anchor		 = index;
			_lastClicked = index;
		}

		_dragging		= true;
		_dragStartIndex = index;
		_dragAddMode	= not selected;
	}

	void Sequence::mouseMoveEvent( QMouseEvent * p_e )
	{
		using namespace App;
		using namespace App::Action;

		if ( not _dragging )
		{
			return;
		}

		auto opt = _indexFromPos( p_e->pos() );
		if ( not opt )
		{
			return;
		}

		Index index = *opt;

		auto & reg		 = App::REG();
		auto & selection = reg.get<System::Selection>( _system );

		Index a = Util::Math::min( _dragStartIndex, index );
		Index b = Util::Math::max( _dragStartIndex, index );

		// Normal.
		if ( not( p_e->modifiers() & Qt::ControlModifier ) )
		{
			ACTION().execute<Selection::Clear>( _system );
			ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>(
				_system, Core::Struct::IndexRange { a, b }
			);
		}
		// CTRL.
		else
		{
			if ( _dragAddMode )
			{
				ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>(
					_system, Core::Struct::IndexRange { a, b }
				);
			}
			else
			{
				ACTION().execute<Selection::SetSelected<Scene::E_ITEM::RESIDUE>>(
					_system, Core::Struct::IndexRange { a, b }, false
				);
			}
		}
	}

	void Sequence::mouseReleaseEvent( QMouseEvent * p_e )
	{
		_dragging		= false;
		_dragStartIndex = INVALID_INDEX;
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { _updateScrollBars(); }

	void Sequence::_updateScrollBars()
	{
		auto &	   system		= App::REG().get<Core::Struct::System>( _system );
		const uint contentWidth = uint( system.getResidueCount() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}

	std::optional<Index> Sequence::_indexFromPos( const QPoint & p )
	{
		const int xOffset = horizontalScrollBar()->value();
		const int clickX  = p.x() + xOffset;
		auto &	  system  = App::REG().get<Core::Struct::System>( _system );

		Index index = clickX / SEQ_CHAR_WIDTH;
		if ( index >= system.getResidueCount() )
		{
			return std::nullopt;
		}

		// Y-range check
		const int y			= p.y();
		const int topMargin = contentsMargins().top();
		if ( y <= topMargin + SEQ_CHAR_HEIGHT || y >= topMargin + SEQ_CHAR_HEIGHT * 2 )
		{
			return std::nullopt;
		}

		return index;
	}
} // namespace VTX::UI::QT::Widget
