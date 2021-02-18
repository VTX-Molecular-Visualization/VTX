#include "sequence_dataset_residue.hpp"
#include "style.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
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
		{
			const Model::Residue * const residue = molecule->getResidue( chainFirstIndex + _startResidueIndex + i );
			sequenceString[ i ]					 = residue->getSymbolShort()[ 0 ];
		}

		p_string.append( sequenceString );
	}

	void SequenceDisplayDataset_Residue::appendToScale( QString &  p_scale,
														uint &	   p_lastIndexCharWritten,
														const bool p_startBloc ) const
	{
		uint currentIndexChar;
		uint currentLocalIndexResidue;

		const int originalIndexFirstResidue
			= _linkedChain.getMoleculePtr()
				  ->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex )
				  ->getIndexInOriginalChain();

		if ( p_startBloc )
		{
			const std::string firstResidueStr = std::to_string( originalIndexFirstResidue );
			p_lastIndexCharWritten			  = _drawInScale( p_scale, firstResidueStr, _startIndexChar, false );

			const std::string strSecondIndex	 = std::to_string( originalIndexFirstResidue + 1 );
			const uint		  nextValidCharIndex = p_lastIndexCharWritten + ( (uint)strSecondIndex.size() / 2 ) + 2;
			const int nextValidOriginalIndex	 = originalIndexFirstResidue + ( nextValidCharIndex - _startIndexChar );

			const uint step = _getStepToNextValidIndex( nextValidOriginalIndex );

			currentIndexChar		 = nextValidCharIndex + step;
			currentLocalIndexResidue = currentIndexChar - _startIndexChar;
		}
		else
		{
			const uint step = _getStepToNextValidIndex( originalIndexFirstResidue );

			currentIndexChar		 = _startIndexChar + step;
			currentLocalIndexResidue = step;

			const int charOffset
				= int( std::to_string( originalIndexFirstResidue + currentLocalIndexResidue ).size() ) / 2;

			if ( ( currentIndexChar - charOffset ) <= ( p_lastIndexCharWritten + 1 ) )
			{
				currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
				currentLocalIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP;
			}
		}

		const uint							  endIndex = _endResidueIndex - _startResidueIndex;
		const std::vector<Model::Residue *> & residues = _linkedChain.getMoleculePtr()->getResidues();
		const uint residueIndexOffset				   = _linkedChain.getIndexFirstResidue() + _startResidueIndex;
		for ( ; currentLocalIndexResidue <= endIndex; currentLocalIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP )
		{
			// const int currentOriginalResidueIndex = originalIndexFirstResidue + currentLocalIndexResidue;
			const int currentOriginalResidueIndex
				= residues[ residueIndexOffset + currentLocalIndexResidue ]->getIndexInOriginalChain();

			std::string strIndex   = std::to_string( currentOriginalResidueIndex );
			p_lastIndexCharWritten = _drawInScale( p_scale, strIndex, currentIndexChar, true );

			currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
		}
	}

	Model::Residue * const SequenceDisplayDataset_Residue::getResidueAtCharIndex( const uint p_charIndex )
	{
		const uint residueIndex
			= _linkedChain.getIndexFirstResidue() + _startResidueIndex + ( p_charIndex - _startIndexChar );
		return _linkedChain.getMoleculePtr()->getResidue( residueIndex );
	}

	bool SequenceDisplayDataset_Residue::isResidueInScope( const uint p_residueIndex ) const
	{
		return _startResidueIndex <= p_residueIndex && p_residueIndex <= _endResidueIndex;
	}

	uint SequenceDisplayDataset_Residue::getCharIndexOfResidue( const uint p_residueIndex ) const
	{
		return _startIndexChar + p_residueIndex - _startResidueIndex;
	}
} // namespace VTX::UI::Widget::Sequence::Dataset
