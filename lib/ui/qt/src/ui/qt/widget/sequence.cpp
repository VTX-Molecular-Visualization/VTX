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

	namespace
	{
		struct Residue
		{
			std::string_view chainName;
			Index			 chainIndex		 = 0;
			Index			 ruleDrawNumber	 = 0;
			bool			 selected		 = false;
			char			 oneLetterSymbol = '-';
		};

		class ResidueIterator
		{
		  public:
			using iterator_category = std::contiguous_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= Residue;
			using pointer			= value_type *; // or also value_type*
			using reference			= value_type &; // or also value_type&

			ResidueIterator( std::function<Residue( Index )> p_residueGetter, Index p_idx ) :
				_getter( std::move( p_residueGetter ) ), _idx( p_idx )
			{
			}

			reference operator*()
			{
				if ( not _obj.has_value() )
				{
					_obj = _getter( _idx );
				}
				return *_obj;
			}

			pointer operator->()
			{
				if ( not _obj.has_value() )
				{
					_obj = _getter( _idx );
				}
				return &_obj.value();
			}

			// Prefix increment
			ResidueIterator & operator++()
			{
				_idx++;
				if ( _obj.has_value() )
				{
					_obj.reset();
				}
				return *this;
			}

			// Postfix increment
			ResidueIterator operator++( int )
			{
				ResidueIterator tmp = *this;
				++( *this );
				return tmp;
			}

			Index operator-( const ResidueIterator & p_other ) const noexcept { return _idx - p_other._idx; }

			friend bool operator<( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx < b._idx; };

			friend bool operator>( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx > b._idx; };

			friend bool operator<=( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx <= b._idx; };

			friend bool operator>=( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx >= b._idx; };

			friend bool operator==( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx == b._idx; };

			friend bool operator!=( const ResidueIterator & a, const ResidueIterator & b ) { return a._idx != b._idx; };

		  private:
			Index							_idx = 0;
			std::function<Residue( Index )> _getter;
			std::optional<Residue>			_obj;
		};

	} // namespace

	/**
	 * @brief Class responsible for providing iterator over residues who's behavior follow the current Mode.
	 */
	class Sequence::ResidueSequencer
	{
	  public:
		ResidueSequencer( Entity p_entity ) :
			_entity( std::move( p_entity ) ),
			_mode( static_cast<Sequence::Mode>( SETTINGS().value( SETTING_KEY_SEQUENCE_VTX_RESID ).toInt() ) )
		{
			auto & topology = App::REG().get<Core::Struct::Topology>( _entity );

			for ( Index it_vtxResIdx = 0; it_vtxResIdx < topology.getResidueCount(); it_vtxResIdx++ )
			{
				if ( it_vtxResIdx > 0
					 and topology.residueOriginalIds[ it_vtxResIdx ]
							 > topology
								   .residueOriginalIds[ it_vtxResIdx - 1 ] ) // case where the resid restart. Either
																			 // from a new chain, or some other reason.
				{
					_oIdData.ruleSize += topology.residueOriginalIds[ it_vtxResIdx ]
										 - topology.residueOriginalIds[ it_vtxResIdx - 1 ];
				}
				else
				{
					_oIdData.ruleSize += topology.residueOriginalIds[ it_vtxResIdx ];
				}

				_oIdData.ruleIndex2residueIndex[ _oIdData.ruleSize ]
					= std::make_pair( topology.residueChainIndexes[ it_vtxResIdx ], it_vtxResIdx );
			}
		}

		void set( Sequence::Mode p_mode ) { _mode = std::move( p_mode ); }

		Index residuesNumber() const { return App::REG().get<Core::Struct::Topology>( _entity ).getResidueCount(); }

		Index ruleSize() const
		{
			switch ( _mode )
			{
			case Sequence::Mode::contiguousResId: return residuesNumber();
			default: return _oIdData.ruleSize;
			}
		}

		ResidueIterator begin( Index p_startIndex )
		{
			return ResidueIterator( [ this ]( Index p_ ) { return this->residueAt( p_ ); }, p_startIndex );
		}

		ResidueIterator end( Index p_lastIndex )
		{
			return ResidueIterator( [ this ]( Index p_ ) { return this->residueAt( p_ ); }, p_lastIndex );
		}

		std::optional<Index> residueIndexfromRuleIndex( Index p_ruleIndex ) const
		{
			switch ( _mode )
			{
			case Sequence::Mode::contiguousResId: return p_ruleIndex;
			default:
				if ( _oIdData.ruleIndex2residueIndex.contains( p_ruleIndex ) )
				{
					return _oIdData.ruleIndex2residueIndex.at( p_ruleIndex ).second;
				}
				else
				{
					return std::nullopt;
				}
			}
		}

		Residue residueAt( Index p_index ) const
		{
			Residue out;
			auto &	topology = App::REG().get<Core::Struct::Topology>( _entity );

			switch ( _mode )
			{
			case Sequence::Mode::contiguousResId:
			{
				out.selected = App::Helper::System::getSelectionState(
								   { _entity, Core::Struct::E_SYSTEM_ITEM::RESIDUE, p_index }
							   )
							   != App::System::E_SELECTION_STATE::NONE;

				if ( p_index < topology.residueSymbols.size() )
				{
					const auto symbol	= topology.residueSymbols[ p_index ];
					const auto name		= Core::ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ];
					out.oneLetterSymbol = name.at( 0 );
				}
				if ( p_index < topology.residueChainIndexes.size() )
				{
					out.chainIndex = topology.residueChainIndexes[ p_index ];
				}
				if ( out.chainIndex < topology.chainNames.size() )
				{
					auto & chainName = topology.chainNames[ out.chainIndex ];
					out.chainName	 = std::string_view( chainName.data(), chainName.size() );
				}
			}
			default:
			{
				out.ruleDrawNumber = p_index;
				if ( _oIdData.ruleIndex2residueIndex.contains( p_index ) )
				{
					auto & pair		 = _oIdData.ruleIndex2residueIndex.at( p_index );
					out.chainIndex	 = pair.first;
					auto & chainName = topology.chainNames[ out.chainIndex ];
					out.chainName	 = std::string_view( chainName.data(), chainName.size() );
					if ( pair.second < topology.residueSymbols.size() )
					{
						const auto symbol	= topology.residueSymbols[ pair.second ];
						const auto name		= Core::ChemDB::Residue::SYMBOL_SHORT_STR[ int( symbol ) ];
						out.oneLetterSymbol = name.at( 0 );
					}
					out.selected = App::Helper::System::getSelectionState(
									   { _entity, Core::Struct::E_SYSTEM_ITEM::RESIDUE, p_index }
								   )
								   != App::System::E_SELECTION_STATE::NONE;
				}
				else
				{
					// Case of a filler residue, noted '-' and owned by the chain of the previous residue
					Index currentIndex = p_index;
					while ( currentIndex > 0 and not _oIdData.ruleIndex2residueIndex.contains( currentIndex ) )
					{
						currentIndex--;
					}
					if ( _oIdData.ruleIndex2residueIndex.contains( currentIndex ) )
					{
						auto & pair		 = _oIdData.ruleIndex2residueIndex.at( currentIndex );
						out.chainIndex	 = pair.first;
						auto & chainName = topology.chainNames[ out.chainIndex ];
						out.chainName	 = std::string_view( chainName.data(), chainName.size() );
					}
				}
			}
			}
			return out;
		}

	  private:
		Entity		   _entity;
		Sequence::Mode _mode;

		struct OriginalResidData
		{
			/**
			 * @brief Link the index used as the ruler for the original resid mode
			 Key : rule index
			 Value : pair<chain id, vtx resid>
			 */
			std::unordered_map<Index, std::pair<Index, Index>> ruleIndex2residueIndex;
			Index											   ruleSize = 0;
		} _oIdData;
	};

	Sequence::Sequence( const Entity p_system, QWidget * p_parent ) :
		QAbstractScrollArea( p_parent ), _system( p_system ), _sequencer( new ResidueSequencer( p_system ) )
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
		auto & metadata			  = reg.get<IO::Metadata>( _system );
		auto & colorLayoutIntance = ECS::getFirstComponent<Scene::ColorLayout>();
		auto & colorlayout		  = reg.get<Renderer::Color::Layout>( colorLayoutIntance.preset );

		const int xOffset = horizontalScrollBar()->value();

		const Index			  startIndex = xOffset / SEQ_CHAR_WIDTH;
		ResidueIterator		  residue	 = _sequencer->begin( startIndex );
		const ResidueIterator end		 = _sequencer->end( startIndex + ( viewport()->width() / SEQ_CHAR_WIDTH ) + 2 );

		if ( end <= residue )
		{
			return;
		}

		int x = -( xOffset % SEQ_CHAR_WIDTH );

		// Label with current chain.
		const Index	  firstChain  = residue->chainIndex;
		const QString headerLabel = QString( "%1/%2" ).arg(
			QString::fromStdString( metadata.pdbIDCode ),
			QString::fromStdString( std::string( residue->chainName.data(), residue->chainName.size() ) )
		);
		painter.setPen( Helper::toQColor( colorlayout.getChainColor( firstChain + 1 ) ) );
		painter.drawText( 0, SEQ_CHAR_HEIGHT, headerLabel );
		const int labelWidth = painter.fontMetrics().horizontalAdvance( headerLabel );

		// Draw the residue sequence.
		Index lastChain = firstChain;
		for ( ; residue != end; ++residue )
		{
			painter.setPen( Helper::toQColor( colorlayout.getChainColor( size_t( residue->chainIndex + 1 ) ) ) );

			// Chain labels.
			int labelChainWidth = 0;
			if ( residue->chainIndex != lastChain )
			{
				if ( x > labelWidth )
				{
					QString chainLabel = QString( "/%1" ).arg(
						QString::fromStdString( std::string( residue->chainName.data(), residue->chainName.size() ) )
					);
					painter.drawText( x, SEQ_CHAR_HEIGHT, chainLabel );
					labelChainWidth = painter.fontMetrics().horizontalAdvance( chainLabel );
				}
				lastChain = residue->chainIndex;
			}

			// Rule.
			const size_t indexInChain = residue->ruleDrawNumber;
			if ( x > labelWidth && x > labelChainWidth && indexInChain % SEQ_RULE_STEP == 0 )
			{
				painter.drawText( x, SEQ_CHAR_HEIGHT, QString::number( indexInChain ) );
			}

			// Selection.
			const QRect cellRect( x - 2, SEQ_CHAR_HEIGHT + 5, SEQ_CHAR_WIDTH, SEQ_CHAR_HEIGHT );
			if ( residue->selected )
			{
				painter.fillRect( cellRect, palette().highlight() );
			}
			else
			{
				painter.fillRect( cellRect, palette().base() );
			}

			// Residue symbol.
			painter.drawText( x, SEQ_CHAR_HEIGHT * 2, QString( residue->oneLetterSymbol ) );

			x += SEQ_CHAR_WIDTH;
		}
	}

	void Sequence::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

	void Sequence::Del::operator()( ResidueSequencer * p_ ) const noexcept { delete p_; }

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
		const uint contentWidth = uint( _sequencer->ruleSize() ) * SEQ_CHAR_WIDTH;
		horizontalScrollBar()->setRange( 0, contentWidth - viewport()->width() );
		horizontalScrollBar()->setPageStep( viewport()->width() );
	}

	std::optional<Index> Sequence::_indexFromPos( const QPoint & p )
	{
		const int xOffset  = horizontalScrollBar()->value();
		const int clickX   = p.x() + xOffset;
		auto &	  topology = App::REG().get<Core::Struct::Topology>( _system );

		Index index = clickX / SEQ_CHAR_WIDTH;
		if ( index > _sequencer->ruleSize() )
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

		return _sequencer->residueIndexfromRuleIndex( index );
	}
} // namespace VTX::UI::QT::Widget
