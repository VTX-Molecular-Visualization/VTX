#include "molecule_sequence_widget.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include "sequence_display_widget.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include <QScrollBar>

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

					_lastResidueHovered = _getResidueAtPos( _startPressPosition );

					if ( _lastResidueHovered != nullptr )
					{
						if ( _isSelected( *_lastResidueHovered ) )
							_deselectResidue( *_lastResidueHovered );
						else
							_selectResidue( *_lastResidueHovered );
					}

					_repaintSelection();
				}

				void MoleculeSequenceWidget::mouseMoveEvent( QMouseEvent * ev )
				{
					ViewItemWidget<Model::Molecule>::mouseMoveEvent( ev );

					if ( !ev->buttons().testFlag( Qt::MouseButton::LeftButton ) )
						return;

					const QPoint	 currentMousePos	   = _scrollAreaContent->mapFromGlobal( ev->globalPos() );
					Model::Residue * currentResidueHovered = _getResidueAtPos( currentMousePos );

					const bool switchStartClickSide = ( _lastDragSelectionPosition.x() < _startPressPosition.x() && currentMousePos.x() > _startPressPosition.x() )
													  || ( _lastDragSelectionPosition.x() > _startPressPosition.x() && currentMousePos.x() < _startPressPosition.x() );

					// If the cursor switch side of the first clicked object, we clear the selection
					if ( switchStartClickSide )
					{
						_clearSelection();
						Model::Residue * const startResidue = _getResidueAtPos( _startPressPosition );

						if ( startResidue != nullptr )
							_selection.emplace_back( startResidue );

						_lastResidueHovered = startResidue;
						_repaintSelection();
					}

					if ( currentResidueHovered == _lastResidueHovered )
					{
						_lastDragSelectionPosition = currentMousePos;
						return;
					}

					if ( currentResidueHovered == nullptr )
					{
						const bool getForwardResidue = _startPressPosition.x() > currentMousePos.x();
						currentResidueHovered		 = _getClosestResidue( currentMousePos, getForwardResidue );
					}

					if ( currentResidueHovered == nullptr )
					{
						_lastDragSelectionPosition = currentMousePos;
						return;
					}

					const bool hoveredResidueAlreadySelected = _isSelected( *currentResidueHovered );

					if ( !hoveredResidueAlreadySelected )
					{
						const bool moveForward = _lastDragSelectionPosition.x() <= currentMousePos.x();

						if ( _lastResidueHovered == nullptr )
						{
							_lastResidueHovered = _getClosestResidue( _lastDragSelectionPosition, moveForward );
							_selectResidue( *_lastResidueHovered );
						}
						if ( moveForward )
						{
							const Model::Chain * const startChain = _lastResidueHovered->getChainPtr();
							const Model::Chain * const endChain	  = currentResidueHovered->getChainPtr();

							for ( uint iChain = startChain->getIndex(); iChain <= endChain->getIndex(); iChain++ )
							{
								const Model::Chain & currentChain = _model->getChain( iChain );
								const uint startResidueIndex	  = ( &currentChain == startChain ) ? ( _lastResidueHovered->getIndex() + 1 ) : currentChain.getIndexFirstResidue();
								const uint endResidueIndex
									= ( &currentChain == endChain ) ? currentResidueHovered->getIndex() : currentChain.getIndexFirstResidue() + currentChain.getResidueCount() - 1;

								for ( uint iResidue = startResidueIndex; iResidue <= endResidueIndex; iResidue++ )
								{
									Model::Residue & currentResidue = _model->getResidue( iResidue );
									_selectResidue( currentResidue );
								}
							}
						}
						else
						{
							const Model::Chain * const startChain = _lastResidueHovered->getChainPtr();
							const Model::Chain * const endChain	  = currentResidueHovered->getChainPtr();

							for ( uint iChain = startChain->getIndex(); iChain >= endChain->getIndex(); iChain-- )
							{
								const Model::Chain & currentChain	   = _model->getChain( iChain );
								const uint			 startResidueIndex = ( &currentChain == startChain ) ? ( _lastResidueHovered->getIndex() - 1 )
																										 : currentChain.getIndexFirstResidue() + currentChain.getResidueCount() - 1;
								const uint			 endResidueIndex   = ( &currentChain == endChain ) ? currentResidueHovered->getIndex() : currentChain.getIndexFirstResidue();

								for ( uint iResidue = startResidueIndex; iResidue >= endResidueIndex; iResidue-- )
								{
									Model::Residue & currentResidue = _model->getResidue( iResidue );
									_selectResidue( currentResidue );

									// prevent uint go to maxSize
									if ( iResidue == 0 )
										break;
								}

								// prevent uint go to maxSize
								if ( iChain == 0 )
									break;
							}
						}
					}
					else
					{
						// Because we add elements in the right order, we can pop the selection to the hovered residue
						while ( _selection.back()->getId() != currentResidueHovered->getId() )
						{
							_selection.pop_back();
						}
					}

					_repaintSelection();

					_lastDragSelectionPosition = currentMousePos;
					_lastResidueHovered		   = currentResidueHovered;
				}

				void MoleculeSequenceWidget::mouseReleaseEvent( QMouseEvent * ev )
				{
					ViewItemWidget<Model::Molecule>::mouseReleaseEvent( ev );
					_lastResidueHovered = nullptr;
				}

				bool MoleculeSequenceWidget::_isSelected( const Model::Residue & residue ) const
				{
					for ( auto it : _selection )
					{
						if ( it->getId() == residue.getId() )
							return true;
					}

					return false;
				}
				void MoleculeSequenceWidget::_selectResidue( Model::Residue & p_residue )
				{
					_selection.emplace_back( &p_residue );
					VTX::Selection::SelectionManager::get().getSelectionModel().selectResidue( p_residue );
				}
				void MoleculeSequenceWidget::_deselectResidue( Model::Residue & p_residue )
				{
					VTX::Selection::SelectionManager::get().getSelectionModel().unselectResidue( p_residue );

					for ( int i = 0; i < _selection.size(); i++ )
					{
						if ( _selection[ i ]->getId() == p_residue.getId() )
						{
							_selection.erase( _selection.cbegin() + i );
							return;
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
				Model::Residue * const MoleculeSequenceWidget::_getClosestResidue( const QPoint & p_pos, const bool p_next ) const
				{
					Model::Residue *			res			  = nullptr;
					const ChainSequenceWidget * previousChain = nullptr;

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

				void MoleculeSequenceWidget::_repaintSelection() const
				{
					for ( auto it : _chainDisplayWidgets )
					{
						it->updateSelection( _selection );
					}
				}

				void MoleculeSequenceWidget::_clearSelection() { _selection.clear(); }
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
