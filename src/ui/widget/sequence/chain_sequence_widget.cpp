#include "chain_sequence_widget.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
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
					const Model::Molecule * const molecule = _model->getMoleculePtr();

					for ( uint residueIndex : _sequenceDisplayWidget->getSelection() )
					{
						const Model::Residue residue = molecule->getResidue( residueIndex );
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

					const int residueCount = _model->getResidueCount();

					if ( residueCount <= 0 )
						return;

					const int		  startIndex		= _model->getMoleculePtr()->getResidue( _model->getIndexFirstResidue() ).getIndexInOriginalChain();
					const std::string firstIndexStr		= std::to_string( startIndex );
					const int		  firstIndexStrSize = (int)firstIndexStr.size();

					const int		  lastIndex					  = startIndex + residueCount - 1;
					const std::string lastIndexStr				  = std::to_string( lastIndex );
					const int		  lastIndexForwardOffsetIndex = (int)( lastIndexStr.size() / 2 );

					const int scaleTxtLength = residueCount + ( lastIndexForwardOffsetIndex - ( lastIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) );

					QString scaleTxt = QString( scaleTxtLength, ' ' );

					// We display the index of the first label (in original chain)
					for ( int i = 0; i < firstIndexStrSize; i++ )
						scaleTxt[ i ] = firstIndexStr[ i ];

					// Then, the second one will be the next multiple of Style::SEQUENCE_CHAIN_SCALE_STEP which does not overlay or stick with the chars of the first index
					int secondIndex = Style::SEQUENCE_CHAIN_SCALE_STEP - ( startIndex % Style::SEQUENCE_CHAIN_SCALE_STEP );
					if ( secondIndex <= ( firstIndexStrSize + ( firstIndexStrSize / 2 ) ) )
						secondIndex += Style::SEQUENCE_CHAIN_SCALE_STEP;

					// After that, we draw a label every Style::SEQUENCE_CHAIN_SCALE_STEP
					for ( int i = secondIndex; i < residueCount; i += Style::SEQUENCE_CHAIN_SCALE_STEP )
					{
						const std::string indexTxt	   = std::to_string( startIndex + i );
						const int		  indexTxtSize = (int)indexTxt.size();
						const int		  indexOffset  = ( indexTxtSize - 1 ) / 2;

						for ( int j = 0; j < indexTxtSize; j++ )
							scaleTxt[ i + j - indexOffset ] = indexTxt[ j ];
					}

					_scaleWidget->setText( scaleTxt );
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
