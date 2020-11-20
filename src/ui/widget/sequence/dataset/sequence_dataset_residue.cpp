#include "sequence_dataset_residue.hpp"
#include "style.hpp"

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
					SequenceDisplayDataset_Residue::SequenceDisplayDataset_Residue( const Model::Chain & p_chain,
																					const uint			 p_startIndexChar,
																					const uint			 p_startResidueIndex,
																					const uint			 p_endResidueIndex ) :
						SequenceDisplayDataset( p_startIndexChar, p_endResidueIndex - p_startResidueIndex + 1 ),
						_startResidueIndex( p_startResidueIndex ), _endResidueIndex( p_endResidueIndex ), _linkedChain( p_chain ) {};

					void SequenceDisplayDataset_Residue::appendToSequence( QString & p_string ) const
					{
						const uint size			  = _endResidueIndex - _startResidueIndex + 1;
						QString	   sequenceString = QString( size, ' ' );

						const Model::Molecule * const molecule		  = _linkedChain.getMoleculePtr();
						const uint					  chainFirstIndex = _linkedChain.getIndexFirstResidue();

						for ( uint i = 0; i < size; i++ )
							sequenceString[ i ] = molecule->getResidue( chainFirstIndex + _startResidueIndex + i ).getSymbolShort()[ 0 ];

						p_string.append( sequenceString );
					}

					void SequenceDisplayDataset_Residue::appendToScale( QString & p_scale, const bool p_startBloc ) const
					{
						uint currentIndexChar;
						uint currentLocalIndexResidue;

						const uint firstResidue = _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex ).getIndexInOriginalChain();

						if ( p_startBloc )
						{
							const std::string firstResidueStr	 = std::to_string( firstResidue );
							const uint		  lastCharFirstIndex = _drawInScale( p_scale, firstResidueStr, _startIndexChar, false );

							const std::string strSecondIndex		= std::to_string( firstResidue + 1 );
							const uint		  nextValidIndex		= lastCharFirstIndex + ( (uint)strSecondIndex.size() / 2 ) + 1;
							const uint		  residueNextValidIndex = firstResidue + ( nextValidIndex - _startIndexChar );

							const uint moduloStep = residueNextValidIndex % Style::SEQUENCE_CHAIN_SCALE_STEP;
							const uint step		  = moduloStep == 0 ? 0 : ( Style::SEQUENCE_CHAIN_SCALE_STEP - moduloStep );

							currentIndexChar		 = nextValidIndex + step;
							currentLocalIndexResidue = _startResidueIndex + ( currentIndexChar - _startIndexChar );
						}
						else
						{
							uint moduloStep = firstResidue % Style::SEQUENCE_CHAIN_SCALE_STEP;
							uint step		= moduloStep == 0 ? 0 : Style::SEQUENCE_CHAIN_SCALE_STEP - moduloStep;

							currentIndexChar		 = _startIndexChar + step;
							currentLocalIndexResidue = _startResidueIndex + step;
						}

						for ( ; currentLocalIndexResidue <= _endResidueIndex; currentLocalIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP )
						{
							const uint currentResidueIndex
								= _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + currentLocalIndexResidue ).getIndexInOriginalChain();

							std::string strIndex = std::to_string( currentResidueIndex );
							_drawInScale( p_scale, strIndex, currentIndexChar, true );

							currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
						}
					}

					Model::Residue * const SequenceDisplayDataset_Residue::getResidueAtCharIndex( const uint p_charIndex )
					{
						const uint firstIndexInOriginlaChain = _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex ).getIndex();
						uint	   residueIndex				 = firstIndexInOriginlaChain + p_charIndex - _startIndexChar;
						return &( _linkedChain.getMoleculePtr()->getResidue( residueIndex ) );
					}

					bool SequenceDisplayDataset_Residue::isResidueInScope( const uint p_residueIndex ) const
					{
						return _startResidueIndex <= p_residueIndex && p_residueIndex <= _endResidueIndex;
					}

					uint SequenceDisplayDataset_Residue::getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar + p_residueIndex - _startResidueIndex; }
				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
