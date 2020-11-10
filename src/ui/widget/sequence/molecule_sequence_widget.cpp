#include "molecule_sequence_widget.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include "sequence_display_widget.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QScrollBar>
#include <algorithm>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				MoleculeSequenceWidget::MoleculeSequenceWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) { _registerEvent( Event::Global::SELECTION_CHANGE ); }

				void MoleculeSequenceWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::SELECTION_CHANGE ) {}
				}

				void MoleculeSequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 0, 0, 0, 0 );

					_sequenceLabel = new QLabel( this );
					_sequenceLabel->setAlignment( Qt::AlignmentFlag::AlignTop );
					_sequenceLabel->setFont( Style::SEQUENCE_DISPLAY_FONT );

					_scrollAreaContent = new QWidget();
					_scrollAreaContent->setContentsMargins( 0, 0, 0, 0 );
					_sequenceLayout = new QHBoxLayout( _scrollAreaContent );
					_sequenceLayout->setSizeConstraint( QLayout::SetMinAndMaxSize );
					_sequenceLayout->setContentsMargins( 0, 0, 0, 0 );
					_sequenceLayout->setSpacing( 5 );
					_sequenceLayout->addStretch( 1000 );

					_scrollArea = new QScrollArea( this );
					_scrollArea->setContentsMargins( 0, 0, 0, 0 );
					_scrollArea->setWidget( _scrollAreaContent );
					_scrollArea->setFrameShape( QFrame::Shape::NoFrame );
					_scrollArea->horizontalScrollBar()->setObjectName( "SequenceScrollbar" );

					QHBoxLayout * const layout = new QHBoxLayout( this );
					layout->setContentsMargins( 0, 0, 0, 0 );

					layout->addWidget( _sequenceLabel );
					layout->addWidget( _scrollArea );
				}

				void MoleculeSequenceWidget::_setupSlots()
				{
					const QScrollBar * const scrollBar = _scrollArea->horizontalScrollBar();
					connect( scrollBar, &QScrollBar::valueChanged, this, &MoleculeSequenceWidget::_onScrollBarValueChanged );
				}

				void MoleculeSequenceWidget::_onScrollBarValueChanged()
				{
					const QScrollBar * const scrollBar = _scrollArea->horizontalScrollBar();
					const int				 value	   = scrollBar->value();

					// Check first visible
					for ( auto it = _chainDisplayWidgets.rbegin(); it != _chainDisplayWidgets.rend(); it++ )
					{
						const QPoint pos = ( *it )->pos();

						if ( pos.x() < value )
						{
							_updateLabelName( *( ( *it )->getModel() ) );
							break;
						}
					}
				}

				void MoleculeSequenceWidget::_updateLabelName( const Model::Chain & p_currentChainDisplayed )
				{
					_sequenceLabel->setText( QString::fromStdString( _model->getPdbIdCode() + Style::SEQUENCE_CHAIN_NAME_SEPARATOR + p_currentChainDisplayed.getName()
																	 + Style::SEQUENCE_CHAIN_NAME_SEPARATOR ) );
				}

				void MoleculeSequenceWidget::localize() {}
				void MoleculeSequenceWidget::refresh()
				{
					const int chainCount = _model->getChainCount();
					if ( chainCount <= 0 )
						_sequenceLabel->setText( QString::fromStdString( _model->getPdbIdCode() ) );
					else
						_updateLabelName( _model->getChain( 0 ) );

					_chainDisplayWidgets = std::vector<ChainSequenceWidget *>();
					_chainDisplayWidgets.reserve( chainCount );

					for ( Model::Chain * const chain : _model->getChains() )
					{
						QLabel * const chainNameWidget = new QLabel( this );
						chainNameWidget->setAlignment( Qt::AlignmentFlag::AlignTop );
						chainNameWidget->setFont( Style::SEQUENCE_DISPLAY_FONT );
						chainNameWidget->setText( QString::fromStdString( Style::SEQUENCE_CHAIN_NAME_SEPARATOR + chain->getName() + Style::SEQUENCE_CHAIN_NAME_SEPARATOR ) );

						ChainSequenceWidget * const chainSequenceDisplay = WidgetFactory::get().getWidget<ChainSequenceWidget>( this, "chainSequenceWidget" );
						_chainDisplayWidgets.emplace_back( chainSequenceDisplay );

						chainSequenceDisplay->setModel( chain );

						_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainNameWidget );
						_sequenceLayout->insertWidget( _sequenceLayout->count() - 1, chainSequenceDisplay );
					}
				}

				void MoleculeSequenceWidget::mousePressEvent( QMouseEvent * ev )
				{
					ViewItemWidget<Model::Molecule>::mousePressEvent( ev );

					if ( !ev->buttons().testFlag( Qt::MouseButton::LeftButton ) )
						return;

					const bool appendToSelection = ev->modifiers() & ( Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier );

					if ( !appendToSelection )
						_clearSelection();

					_startPressPosition		   = _scrollAreaContent->mapFromGlobal( ev->globalPos() );
					_lastDragSelectionPosition = _startPressPosition;

					Model::Residue * const residueHovered = _getResidueAtPos( _startPressPosition );

					if ( residueHovered != nullptr )
					{
						if ( _isSelected( residueHovered ) )
							_unselect( residueHovered );
						else
							_select( residueHovered );
					}

					_lastResidueHovered = residueHovered;
					_repaintSelection();
				}

				void MoleculeSequenceWidget::mouseMoveEvent( QMouseEvent * ev )
				{
					ViewItemWidget<Model::Molecule>::mouseMoveEvent( ev );

					if ( !ev->buttons().testFlag( Qt::MouseButton::LeftButton ) )
						return;

					_frameSelection.clear();

					const QPoint	 currentMousePos	   = _scrollAreaContent->mapFromGlobal( ev->globalPos() );
					Model::Residue * currentResidueHovered = _getResidueAtPos( currentMousePos );

					if ( currentResidueHovered == _lastResidueHovered )
					{
						_lastDragSelectionPosition = currentMousePos;
						return;
					}

					const bool switchSideFromStartClick = ( _lastDragSelectionPosition.x() < _startPressPosition.x() && currentMousePos.x() > _startPressPosition.x() )
														  || ( _lastDragSelectionPosition.x() > _startPressPosition.x() && currentMousePos.x() < _startPressPosition.x() );

					const bool cursorInFrontOfStartClick = currentMousePos.x() > _startPressPosition.x();
					const bool cursorMoveForward		 = currentMousePos.x() >= _lastDragSelectionPosition.x();

					const Model::Residue * const closestLastResidueHovered = _getClosestResidue( _lastDragSelectionPosition, cursorMoveForward );

					// If the cursor switch side of the first clicked object, we clear the selection
					if ( switchSideFromStartClick )
					{
						const Model::Residue * const startResidue = _getResidueAtPos( _startPressPosition );

						const Model::Residue * closestResidueCurrentlyHovered = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick, true );
						const Model::Residue * closestOfStart				  = _getClosestResidue( _startPressPosition, !cursorInFrontOfStartClick, true );

						const Model::Residue * fromResidue = _compareResiduePos( *closestLastResidueHovered, *closestOfStart ) <= 0 ? closestLastResidueHovered : closestOfStart;
						const Model::Residue * toResidue   = _compareResiduePos( *closestLastResidueHovered, *closestOfStart ) > 0 ? closestLastResidueHovered : closestOfStart;

						_getFromTo( *fromResidue, *toResidue, &_frameSelection );
						_unselect( _frameSelection );

						_frameSelection.clear();

						if ( !( startResidue == nullptr && currentResidueHovered == nullptr && closestOfStart == closestResidueCurrentlyHovered ) )
						{
							closestOfStart = _getClosestResidue( _startPressPosition, cursorInFrontOfStartClick, true );

							fromResidue = _compareResiduePos( *closestOfStart, *closestResidueCurrentlyHovered ) <= 0 ? closestOfStart : closestResidueCurrentlyHovered;
							toResidue	= _compareResiduePos( *closestOfStart, *closestResidueCurrentlyHovered ) > 0 ? closestOfStart : closestResidueCurrentlyHovered;
							_getFromTo( *fromResidue, *toResidue, &_frameSelection );

							_select( _frameSelection );
						}
					}
					else
					{
						const bool addToSelection = ( cursorInFrontOfStartClick && cursorMoveForward ) || ( !cursorInFrontOfStartClick && !cursorMoveForward );

						const Model::Residue * fromResidue;
						const Model::Residue * toResidue;

						if ( cursorMoveForward )
						{
							fromResidue = closestLastResidueHovered;

							toResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick );
							if ( !addToSelection )
								toResidue = _getPreviousResidue( *toResidue );
						}
						else
						{
							fromResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick );
							if ( !addToSelection )
							{
								if ( fromResidue == nullptr )
									fromResidue = _getClosestResidue( currentMousePos, !cursorInFrontOfStartClick, true );
								else
									fromResidue = _getNextResidue( *fromResidue );
							}

							toResidue = closestLastResidueHovered;
						}

						_getFromTo( *fromResidue, *toResidue, &_frameSelection );

						if ( addToSelection )
							_select( _frameSelection );
						else
							_unselect( _frameSelection );
					}

					_repaintSelection();

					_lastDragSelectionPosition = currentMousePos;
					_lastResidueHovered		   = currentResidueHovered;
				}

				Model::Residue * const MoleculeSequenceWidget::_getPreviousResidue( const Model::Residue & p_residue ) const
				{
					const Model::Chain * const chain			 = p_residue.getChainPtr();
					const bool				   isFirstOfItsChain = p_residue.getIndex() == chain->getIndexFirstResidue();

					if ( isFirstOfItsChain )
					{
						if ( chain->getIndex() > 0 )
						{
							const Model::Chain & previousChain = _model->getChain( chain->getIndex() - 1 );
							return &( _model->getResidue( previousChain.getIndexLastResidue() ) );
						}
						else
						{
							return &( _model->getResidue( p_residue.getIndex() ) );
						}
					}
					else
					{
						return &( _model->getResidue( p_residue.getIndex() - 1 ) );
					}
				}
				Model::Residue * const MoleculeSequenceWidget::_getNextResidue( const Model::Residue & p_residue ) const
				{
					const Model::Chain * const chain			= p_residue.getChainPtr();
					const bool				   isLastOfItsChain = p_residue.getIndex() == ( chain->getIndexLastResidue() );
					if ( isLastOfItsChain )
					{
						if ( chain->getIndex() < ( _model->getChainCount() - 1 ) )
						{
							const Model::Chain & nextChain = _model->getChain( chain->getIndex() + 1 );
							return &( _model->getResidue( nextChain.getIndexFirstResidue() ) );
						}
						else
						{
							return &( _model->getResidue( p_residue.getIndex() ) );
						}
					}
					else
					{
						return &( _model->getResidue( p_residue.getIndex() + 1 ) );
					}
				}

				int MoleculeSequenceWidget::_compareResiduePos( const Model::Residue & p_lhs, const Model::Residue & p_rhs ) const
				{
					const uint indexChainLhs = p_lhs.getChainPtr()->getIndex();
					const uint indexChainRhs = p_rhs.getChainPtr()->getIndex();

					if ( indexChainLhs < indexChainRhs )
						return -1;
					else if ( indexChainLhs > indexChainRhs )
						return 1;
					else
					{
						const uint indexLhs = p_lhs.getIndex();
						const uint indexRhs = p_rhs.getIndex();

						if ( indexLhs < indexRhs )
							return -1;
						else if ( indexLhs > indexRhs )
							return 1;
						else
							return 0;
					}
				}
				void MoleculeSequenceWidget::_getFromTo( const Model::Residue & p_from, const Model::Residue & p_to, std::vector<Model::Residue *> * const _container ) const
				{
					const Model::Residue & startResidue = p_from;
					const Model::Residue & endResidue	= p_to;

					const Model::Chain * const startChain = startResidue.getChainPtr();
					const Model::Chain * const endChain	  = endResidue.getChainPtr();

					for ( uint iChain = startChain->getIndex(); iChain <= endChain->getIndex(); iChain++ )
					{
						const Model::Chain & currentChain	   = _model->getChain( iChain );
						const uint			 startResidueIndex = ( &currentChain == startChain ) ? startResidue.getIndex() : currentChain.getIndexFirstResidue();
						const uint			 endResidueIndex   = ( &currentChain == endChain ) ? endResidue.getIndex() : currentChain.getIndexLastResidue();

						for ( uint iResidue = startResidueIndex; iResidue <= endResidueIndex; iResidue++ )
						{
							Model::Residue & currentResidue = _model->getResidue( iResidue );
							_container->emplace_back( &currentResidue );
						}
					}
				}
				Model::Residue * const MoleculeSequenceWidget::_getResidueAtPos( const QPoint & p_pos ) const
				{
					Model::Residue * res = nullptr;

					for ( auto it : _chainDisplayWidgets )
					{
						const qreal chainDisplayStartPos = it->pos().x();
						const qreal chainDisplayEndPos	 = chainDisplayStartPos + it->width();

						if ( chainDisplayStartPos <= p_pos.x() && p_pos.x() < chainDisplayEndPos )
						{
							res = &( it->getResidueAtPos( p_pos ) );
							break;
						}
					}

					return res;
				}
				Model::Residue * const MoleculeSequenceWidget::_getClosestResidue( const QPoint & p_pos, const bool p_next, const bool p_forceGetValue ) const
				{
					Model::Residue * res = _getResidueAtPos( p_pos );

					if ( res != nullptr )
						return res;
					else if ( p_forceGetValue )
						res = &( _chainDisplayWidgets[ 0 ]->getFirstResidue() );

					for ( auto it : _chainDisplayWidgets )
					{
						const qreal chainDisplayStartPos = it->pos().x();
						const qreal chainDisplayEndPos	 = chainDisplayStartPos + it->width();

						if ( chainDisplayStartPos > p_pos.x() )
						{
							if ( p_next )
								res = &( it->getFirstResidue() );

							break;
						}
						else if ( !p_next )
							res = &( it->getLastResidue() );
					}

					return res;
				}

				bool MoleculeSequenceWidget::_isSelected( const Model::Residue * const residue ) const
				{
					for ( auto it : _selection )
					{
						if ( it->getId() == residue->getId() )
							return true;
					}

					return false;
				}
				void MoleculeSequenceWidget::_select( const std::vector<Model::Residue *> & p_residues )
				{
					for ( Model::Residue * residue : p_residues )
						_select( residue );
				}
				void MoleculeSequenceWidget::_select( Model::Residue * p_residue )
				{
					if ( !_isSelected( p_residue ) )
					{
						_selection.emplace_back( p_residue );
						VTX::Selection::SelectionManager::get().getSelectionModel().selectResidue( *p_residue );
					}
				}
				void MoleculeSequenceWidget::_unselect( const std::vector<Model::Residue *> & p_residues )
				{
					for ( Model::Residue * residue : p_residues )
					{
						_unselect( residue );
					}
				}
				void MoleculeSequenceWidget::_unselect( const Model::Residue * const p_residue )
				{
					for ( auto it = _selection.begin(); it != _selection.end(); it++ )
					{
						if ( ( *it )->getId() == p_residue->getId() )
						{
							_selection.erase( it );
							VTX::Selection::SelectionManager::get().getSelectionModel().unselectResidue( *p_residue );
							break;
						}
					}
				}
				void MoleculeSequenceWidget::_clearSelection() { _selection.clear(); }

				void MoleculeSequenceWidget::_repaintSelection() const
				{
					for ( auto it : _chainDisplayWidgets )
					{
						it->updateSelection( _selection );
					}
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
