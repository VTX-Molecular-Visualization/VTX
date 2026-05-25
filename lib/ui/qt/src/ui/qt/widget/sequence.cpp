#include "ui/qt/widget/sequence.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/selection.hpp>
#include <app/ecs.hpp>
#include <app/helper/system.hpp>
#include <app/scene/color_layout.hpp>
#include <app/system/selection.hpp>
#include <app/system/uid.hpp>
#include <app/system/visibility.hpp>
#include <core/chemdb/residue.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <renderer/color.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{
	constexpr int SEQ_CHAR_WIDTH  = 12;
	constexpr int SEQ_CHAR_HEIGHT = 18;
	constexpr int SEQ_RULE_STEP	  = 5;

	Sequence::Sequence( const App::Entity p_system, QWidget * p_parent ) :
		QAbstractScrollArea( p_parent ), _system( p_system )
	{
		QFont f( Style::DEFAULT_FONT_FAMILY_SEQUENCE, 10 );
		f.setStyleHint( QFont::Monospace );
		setFont( f );
		setMouseTracking( true );
	}

	void Sequence::paintEvent( QPaintEvent * p_event )
	{
		QPainter painter( viewport() );
		painter.setFont( font() );

		using namespace App;
		using namespace Core::Struct;

		auto & reg				  = REG();
		auto & topology			  = reg.get<Core::Struct::Topology>( _system );
		auto & metadata			  = reg.get<IO::Metadata>( _system );
		auto & uid				  = reg.get<App::System::UID>( _system );
		auto & colorLayoutIntance = ECS::getFirstComponent<Scene::ColorLayout>();
		auto & colorlayout		  = reg.get<Renderer::Color::Layout>( colorLayoutIntance.preset );
		auto & selection		  = reg.get<App::System::Selection>( _system );

		const int	xOffset	   = horizontalScrollBar()->value();
		const Index startIndex = xOffset / SEQ_CHAR_WIDTH;

		Index endIndex
			= Util::Math::min( startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2, topology.getResidueCount() );

		if ( endIndex <= startIndex )
		{
			return;
		}

		int x = -( xOffset % SEQ_CHAR_WIDTH );

		// Label with current chain.
		const Index firstChain = topology.residueChainIndexes[ startIndex ];

		const QString headerLabel = QString( "%1/%2" ).arg(
			QString::fromStdString( metadata.pdbIDCode ), QString::fromStdString( topology.chainNames[ firstChain ] )
		);
		painter.setPen( Helper::toQColor( colorlayout.getChainColor( firstChain + 1 ) ) );
		painter.drawText( 0, SEQ_CHAR_HEIGHT, headerLabel );
		const int labelWidth = painter.fontMetrics().horizontalAdvance( headerLabel );

		// Draw the residue sequence.
		Index lastChain = firstChain;
		for ( Index residue = startIndex; residue < endIndex; ++residue )
		{
			const Index chain = topology.residueChainIndexes[ residue ];
			painter.setPen( Helper::toQColor( colorlayout.getChainColor( size_t( chain + 1 ) ) ) );

			// Chain labels.
			int labelChainWidth = 0;
			if ( chain != lastChain )
			{
				if ( x > labelWidth )
				{
					QString chainLabel = QString( "/%1" ).arg( QString::fromStdString( topology.chainNames[ chain ] ) );
					painter.drawText( x, SEQ_CHAR_HEIGHT, chainLabel );
					labelChainWidth = painter.fontMetrics().horizontalAdvance( chainLabel );
				}
				lastChain = chain;
			}

			// Rule.
			const size_t indexInChain = residue - topology.chainFirstResidues[ chain ] + 1;
			if ( x > labelWidth && x > labelChainWidth && indexInChain % SEQ_RULE_STEP == 0 )
			{
				painter.drawText( x, SEQ_CHAR_HEIGHT, QString::number( indexInChain ) );
			}

			// Selection.
			const QRect cellRect( x - 2, SEQ_CHAR_HEIGHT + 5, SEQ_CHAR_WIDTH, SEQ_CHAR_HEIGHT );
			const bool selected = App::Helper::System::getSelectionState( { _system, E_SYSTEM_ITEM::RESIDUE, residue } )
								  != App::System::E_SELECTION_STATE::NONE;
			if ( selected )
			{
				painter.fillRect( cellRect, palette().highlight() );
			}
			else
			{
				painter.fillRect( cellRect, palette().base() );
			}

			// Residue symbol.
			const auto symbol = topology.residueSymbols[ residue ];
			const auto name	  = Core::ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ];
			painter.drawText( x, SEQ_CHAR_HEIGHT * 2, QString( name.at( 0 ) ) );

			x += SEQ_CHAR_WIDTH;
		}
	}

	void Sequence::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

	void Sequence::mousePressEvent( QMouseEvent * p_e )
	{
		using namespace App;
		using namespace App::Action;
		using namespace Core::Struct;

		if ( p_e->button() == Qt::RightButton && ( p_e->modifiers() & Qt::ControlModifier ) )
		{
			return;
		}

		const bool selectionButton = p_e->button() == Qt::LeftButton || p_e->button() == Qt::RightButton;
		if ( not selectionButton )
		{
			return;
		}

		if ( UI_ACTIONS().isChecked( Action::Selection::LOCK ) )
		{
			return;
		}

		auto opt = _indexFromPos( p_e->pos() );
		if ( not opt )
		{
			return;
		}

		Index index = *opt;

		auto & reg		 = REG();
		auto & selection = reg.get<App::System::Selection>( _system );

		bool shift = p_e->modifiers() & Qt::ShiftModifier;
		bool ctrl  = p_e->modifiers() & Qt::ControlModifier;

		const bool selected = App::Helper::System::getSelectionState( { _system, E_SYSTEM_ITEM::RESIDUE, index } )
							  != App::System::E_SELECTION_STATE::NONE;

		if ( not shift && not ctrl )
		{
			// Normal.
			SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, index );

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

			SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, Core::Struct::IndexRange { a, b } );

			_lastClicked = index;
		}
		else if ( ctrl )
		{
			// CTRL.
			if ( selected )
			{
				SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, index, false, true );
			}
			else
			{
				SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, index, true, true );
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
		using namespace Core::Struct;

		const bool draggingButton = p_e->buttons() & ( Qt::LeftButton | Qt::RightButton );
		if ( not _dragging || not draggingButton )
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
		auto & selection = reg.get<App::System::Selection>( _system );

		Index a = Util::Math::min( _dragStartIndex, index );
		Index b = Util::Math::max( _dragStartIndex, index );

		// Normal.
		if ( not( p_e->modifiers() & Qt::ControlModifier ) )
		{
			SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, Core::Struct::IndexRange { a, b } );
		}
		// CTRL.
		else
		{
			if ( _dragAddMode )
			{
				SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, Core::Struct::IndexRange { a, b }, true, true );
			}
			else
			{
				SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, Core::Struct::IndexRange { a, b }, false, true );
			}
		}
	}

	void Sequence::mouseReleaseEvent( QMouseEvent * p_e )
	{
		_dragging		= false;
		_dragStartIndex = INVALID_INDEX;
	}

	void Sequence::resizeEvent( QResizeEvent * p_event ) { _updateScrollBars(); }

	void Sequence::mouseDoubleClickEvent( QMouseEvent * p_e )
	{
		auto opt = _indexFromPos( p_e->pos() );
		if ( not opt )
		{
			return;
		}

		App::ACTION().execute<App::Action::Camera::Orient>();
	}

	void Sequence::_updateScrollBars()
	{
		auto &	   topology		= App::REG().get<Core::Struct::Topology>( _system );
		const uint contentWidth = uint( topology.getResidueCount() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}

	std::optional<Index> Sequence::_indexFromPos( const QPoint & p )
	{
		const int xOffset  = horizontalScrollBar()->value();
		const int clickX   = p.x() + xOffset;
		auto &	  topology = App::REG().get<Core::Struct::Topology>( _system );

		Index index = clickX / SEQ_CHAR_WIDTH;
		if ( index > topology.getResidueCount() )
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
