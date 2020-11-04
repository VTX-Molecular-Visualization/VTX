#include "chain_sequence_widget.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "unknown_residue_data.hpp"
#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <string>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				ChainSequenceWidget::ChainSequenceWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) { _registerEvent( Event::Global::SELECTION_CHANGE ); }

				void ChainSequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_layout = new QVBoxLayout( this );
					_layout->setContentsMargins( 0, 0, 0, 0 );
					_layout->setSpacing( 1 );
					_sequenceDisplayWidget = new SequenceDisplayWidget( this );
					_sequenceDisplayWidget->setContentsMargins( 0, 0, 0, 0 );
					_scaleWidget = new QLabel();
					_scaleWidget->setContentsMargins( 0, 0, 0, 0 );
					_scaleWidget->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum );
					_scaleWidget->setFont( Style::SEQUENCE_DISPLAY_FONT );
					_scaleWidget->setAlignment( Qt::AlignmentFlag::AlignTop );

					_layout->addWidget( _scaleWidget );
					_layout->addWidget( _sequenceDisplayWidget );
					_layout->addStretch( 1000 );
				}

				void ChainSequenceWidget::_setupSlots() {}

				void ChainSequenceWidget::_onSequenceSelectionChanged() const
				{
					const Model::Molecule * const molecule				= _model->getMoleculePtr();
					const uint					  moleculeResidueOffset = _model->getIndexFirstResidue();

					for ( uint localResidueIndex : _sequenceDisplayWidget->getSelection() )
					{
						const Model::Residue & residue = molecule->getResidue( moleculeResidueOffset + localResidueIndex );
						VTX_INFO( residue.getSymbolName() + " selected." );
					}
				}

				void ChainSequenceWidget::localize()
				{
					connect( _sequenceDisplayWidget, &SequenceDisplayWidget::selectionChanged, this, &ChainSequenceWidget::_onSequenceSelectionChanged );
				}
				void ChainSequenceWidget::refresh()
				{
					_sequenceDisplayWidget->setupSequence( *_model );
					_refreshScale();
				}

				void ChainSequenceWidget::_refreshScale()
				{
					const uint residueCount = _model->getResidueCount();

					if ( residueCount <= 0 )
					{
						_scaleWidget->setText( "" );
						return;
					}

					const uint		  firstResidueIndex	   = _model->getMoleculePtr()->getResidue( _model->getIndexFirstResidue() ).getIndexInOriginalChain();
					const std::string firstResidueIndexStr = std::to_string( firstResidueIndex );

					if ( residueCount == 1 )
					{
						_scaleWidget->setText( QString::fromStdString( firstResidueIndexStr ) );
						return;
					}

					const uint firstIndexStrSize = (uint)firstResidueIndexStr.size();

					// Short symbol size + space
					const std::vector<UnknownResidueData> unknownResidues	  = _sequenceDisplayWidget->getUnknownResiduesPositions();
					uint								  unknownResidueIndex = 0;

					const uint		  lastResidueIndex					 = firstResidueIndex + residueCount - 1;
					const std::string lastResidueIndexStr				 = std::to_string( lastResidueIndex );
					const uint		  lastResidueIndexForwardOffsetIndex = ( uint )( lastResidueIndexStr.size() / 2 );

					uint unknownResidueAdditionalLength = 0;

					for ( auto it = unknownResidues.begin(); it != unknownResidues.end(); it++ )
						unknownResidueAdditionalLength += it->strSize - 1;

					const uint scaleTxtLength
						= residueCount + ( lastResidueIndexForwardOffsetIndex - ( lastResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) ) + unknownResidueAdditionalLength;

					QString scaleTxt = QString( scaleTxtLength, ' ' );

					// We display the index of the first label (in original chain)
					for ( uint i = 0; i < firstIndexStrSize; i++ )
						scaleTxt[ i ] = firstResidueIndexStr[ i ];

					// Then we find the next displayed index which will be the next multiple of Style::SEQUENCE_CHAIN_SCALE_STEP which does not overlay or stick with the chars of
					// the first index
					const uint secondIndex		   = _findSecondIndex( firstResidueIndex, firstIndexStrSize );
					uint	   unknowResidueOffset = 0;
					// and we draw a label every Style::SEQUENCE_CHAIN_SCALE_STEP (verifying unknown symbols that take 4 chars instead of 1)
					for ( uint i = secondIndex; i < residueCount; i += Style::SEQUENCE_CHAIN_SCALE_STEP )
					{
						// We count all unknown symbols passed in the step
						while ( unknownResidueIndex < unknownResidues.size() && i > unknownResidues[ unknownResidueIndex ].residueIndex )
						{
							unknowResidueOffset += unknownResidues[ unknownResidueIndex ].strSize - 1;
							unknownResidueIndex++;
						}

						const std::string indexTxt	   = std::to_string( firstResidueIndex + i );
						const uint		  indexTxtSize = (uint)indexTxt.size();
						const uint		  indexOffset  = ( indexTxtSize - 1 ) / 2;

						for ( uint j = 0; j < indexTxtSize; j++ )
						{
							const uint index  = i + j - indexOffset + unknowResidueOffset;
							scaleTxt[ index ] = indexTxt[ j ];
						}
					}

					_scaleWidget->setText( scaleTxt );
				}

				uint ChainSequenceWidget::_findSecondIndex( const int firstResidueIndex, const int firstIndexStrSize )
				{
					// The second index will be the next multiple of Style::SEQUENCE_CHAIN_SCALE_STEP which does not overlay or stick with the chars of the first index
					uint	   secondIndex		  = Style::SEQUENCE_CHAIN_SCALE_STEP - ( firstResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP );
					const uint secondIndexStrSize = (uint)std::to_string( firstResidueIndex + 1 ).size();

					// first index size + back offset second index + space
					const uint spaceNeededBetweenFirstAndSecondIndex = firstIndexStrSize + ( secondIndexStrSize / 2 ) + 1;
					uint	   spaceBetweenFirstAndSecondIndex		 = 0;

					const std::vector<UnknownResidueData> unknownResidues	  = _sequenceDisplayWidget->getUnknownResiduesPositions();
					uint								  unknownResidueIndex = 0;

					uint strIndex = 0;
					for ( uint residueIndex = 0; residueIndex < secondIndex; residueIndex++ )
					{
						if ( unknownResidues.size() > unknownResidueIndex && unknownResidues[ unknownResidueIndex ].residueIndex == strIndex )
						{
							spaceBetweenFirstAndSecondIndex += unknownResidues[ unknownResidueIndex ].strSize;
							strIndex += unknownResidues[ unknownResidueIndex ].strSize;
							unknownResidueIndex++;
						}
						else
						{
							strIndex += 1;
							spaceBetweenFirstAndSecondIndex += 1;
						}
					}

					if ( spaceBetweenFirstAndSecondIndex < spaceNeededBetweenFirstAndSecondIndex )
						secondIndex += Style::SEQUENCE_CHAIN_SCALE_STEP;

					return secondIndex;
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
