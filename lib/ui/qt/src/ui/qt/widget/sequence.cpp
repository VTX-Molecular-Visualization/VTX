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
#include <ui/qt/action_registry.hpp>
#include <ui/qt/actions.hpp>
#include <ui/qt/events.hpp>
#include <ui/qt/helper.hpp>
#include <ui/qt/menu/selection.hpp>
#include <ui/qt/selection_manager.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/widget/sequence.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Widget
{
	constexpr int SEQ_CHAR_WIDTH  = 12;
	constexpr int SEQ_CHAR_HEIGHT = 18;
	constexpr int SEQ_RULE_STEP	  = 5;

	Sequence::Sequence( const Entity p_system, QWidget * p_parent ) :
		QAbstractScrollArea( p_parent ), _system( p_system ),
		_changeModeConnection( App::HUB().connect<Events::SequenceResIdChanged, &Sequence::_onModeChanged>( this ) )
	{
		QFont f( Style::DEFAULT_FONT_FAMILY_SEQUENCE, 10 );
		f.setStyleHint( QFont::Monospace );
		setFont( f );
		setMouseTracking( true );

		auto & topol		= App::REG().get<Core::Struct::Topology>( p_system );
		Index  currentIndex = 0;
		for ( auto & it_originalResId : topol.residueOriginalIds )
		{
			_originalIndex2VtxIndexMapping[ it_originalResId ] = currentIndex;
			_lastResidueOriginalIndex = std::max( _lastResidueOriginalIndex, it_originalResId );
			currentIndex++;
		}
	}

	std::function<std::optional<Index>( const Index & )> Sequence::_residueIndexConverter() const
	{
		if ( SETTINGS().value( SETTING_KEY_SEQUENCE_VTX_RESID ).toInt()
			 == toUnderlying( Sequence::Mode::contiguousResId ) )
		{
			return [ & ]( const Index & p_index ) { return std::optional<Index>( p_index ); };
		}
		else
		{
			return [ & ]( const Index & p_index )
			{
				if ( _originalIndex2VtxIndexMapping.contains( p_index ) )
				{
					return std::optional<Index>( _originalIndex2VtxIndexMapping.at( p_index ) );
				}
				return std::optional<Index>( std::nullopt );
			};
		}
	}

	std::function<Index( const Core::Struct::Topology &, const Index & )> Sequence::_residueChainResolver() const
	{
		if ( SETTINGS().value( SETTING_KEY_SEQUENCE_VTX_RESID ).toInt()
			 == toUnderlying( Sequence::Mode::contiguousResId ) )
		{
			return [ & ]( const Core::Struct::Topology & p_topol, const Index & p_index ) -> Index { return p_index; };
		}
		else
		{
			return [ &, conv = _residueIndexConverter() ](
					   const Core::Struct::Topology & p_topol, const Index & p_index
				   ) -> Index
			{
				if ( p_index > 0 )
				{
					Index				 closestExistingStartIndex = p_index;
					std::optional<Index> contiguousIndex		   = conv( closestExistingStartIndex );
					while ( closestExistingStartIndex > 0
							&& ( ( not contiguousIndex.has_value() ) || contiguousIndex.value() > 0 ) )
					{
						closestExistingStartIndex--;
						contiguousIndex = conv( closestExistingStartIndex );
					}
					if ( contiguousIndex.has_value() )
					{
						return p_topol.residueChainIndexes.size() > contiguousIndex.value()
								   ? p_topol.residueChainIndexes[ contiguousIndex.value() ]
								   : 0;
					}
				}
				return p_topol.residueChainIndexes.size() > 0 ? p_topol.residueChainIndexes[ 0 ] : 0;
			};
		}
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

		const int											 xOffset	= horizontalScrollBar()->value();
		const Index											 startIndex = xOffset / SEQ_CHAR_WIDTH;
		std::function<std::optional<Index>( const Index & )> screenIndexToTopologyResIndex = _residueIndexConverter();
		auto												 chainResolver				   = _residueChainResolver();

		Index endIndex
			= Util::Math::min( startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2, _lastResidueIndex() );

		if ( endIndex <= startIndex )
		{
			return;
		}

		int x = -( xOffset % SEQ_CHAR_WIDTH );

		// Label with current chain.
		// We need to get the first chain to draw its name. But if the screen resid doesn't exists as a contiguous
		// resid, we need to get back in number until we find one.
		const Index firstChain = chainResolver( topology, startIndex );

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
			const std::optional<Index> contiguousResId = screenIndexToTopologyResIndex( residue );
			const Index				   chain		   = chainResolver( topology, residue );
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
			const size_t indexInChain
				= residue - topology.residueOriginalIds[ topology.chainFirstResidues[ chain ] + 1 ];
			if ( x > labelWidth && x > labelChainWidth && indexInChain % SEQ_RULE_STEP == 0 )
			{
				painter.drawText( x, SEQ_CHAR_HEIGHT, QString::number( indexInChain ) );
			}

			// Selection.
			const QRect cellRect( x - 2, SEQ_CHAR_HEIGHT + 5, SEQ_CHAR_WIDTH, SEQ_CHAR_HEIGHT );
			const bool	selected = contiguousResId.has_value()
								   && App::Helper::System::getSelectionState(
										  { _system, E_SYSTEM_ITEM::RESIDUE, contiguousResId.value() }
									  ) != App::System::E_SELECTION_STATE::NONE;
			if ( selected )
			{
				painter.fillRect( cellRect, palette().highlight() );
			}
			else
			{
				painter.fillRect( cellRect, palette().base() );
			}

			// Residue symbol.
			char oneLetterResidueSymbol = '-';
			if ( contiguousResId.has_value() )
			{
				const auto symbol	   = topology.residueSymbols[ contiguousResId.value() ];
				const auto name		   = Core::ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ];
				oneLetterResidueSymbol = name.at( 0 );
			}
			painter.drawText( x, SEQ_CHAR_HEIGHT * 2, QString( oneLetterResidueSymbol ) );

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

	void Sequence::_onModeChanged( const Events::SequenceResIdChanged & ) { this->repaint(); }

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
		const uint contentWidth = uint( _lastResidueIndex() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}

	Index Sequence::_lastResidueIndex() const
	{
		return SETTINGS().value( SETTING_KEY_SEQUENCE_VTX_RESID ).toInt() == toUnderlying( Mode::contiguousResId )
				   ? _lastResidueOriginalIndex
				   : App::REG().get<Core::Struct::Topology>( _system ).getResidueCount();
	}

	std::optional<Index> Sequence::_indexFromPos( const QPoint & p )
	{
		const int xOffset  = horizontalScrollBar()->value();
		const int clickX   = p.x() + xOffset;
		auto &	  topology = App::REG().get<Core::Struct::Topology>( _system );

		Index index = clickX / SEQ_CHAR_WIDTH;
		if ( index > _lastResidueIndex() )
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
