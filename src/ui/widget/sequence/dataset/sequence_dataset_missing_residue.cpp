#include "sequence_dataset_missing_residue.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				namespace Dataset
				{
					const void SequenceDisplayDataset_MissingResidue::appendToScale( QString & p_scale, const bool p_startBloc ) const
					{
						if ( _isTooLong )
							return;

						int	 currentIndexChar;
						uint currentIndexResidue;

						const uint firstResidue = _startResidueIndexInOriginalChain;
						const uint endResidue	= _startResidueIndexInOriginalChain + _size;

						if ( p_startBloc )
						{
							const std::string firstResidueStr	 = std::to_string( firstResidue );
							const uint		  lastCharFirstIndex = _drawInScale( p_scale, firstResidueStr, _startIndexChar, false );

							const std::string strSecondIndex = std::to_string( firstResidue + 1 );

							const uint nextValidIndex = lastCharFirstIndex + ( (uint)strSecondIndex.size() / 2 ) + 1;

							currentIndexChar	= nextValidIndex + ( ( firstResidue + nextValidIndex ) % Style::SEQUENCE_CHAIN_SCALE_STEP );
							currentIndexResidue = firstResidue + currentIndexChar - _startIndexChar;
						}
						else
						{
							uint moduloStep = firstResidue % Style::SEQUENCE_CHAIN_SCALE_STEP;
							uint step		= moduloStep == 0 ? 0 : Style::SEQUENCE_CHAIN_SCALE_STEP - ( firstResidue % Style::SEQUENCE_CHAIN_SCALE_STEP );

							currentIndexChar	= _startIndexChar + step;
							currentIndexResidue = firstResidue + step;
						}

						for ( ; currentIndexResidue <= endResidue; currentIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP )
						{
							std::string strIndex = std::to_string( currentIndexResidue );
							_drawInScale( p_scale, strIndex, currentIndexChar, true );

							currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
						}
					}

				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
