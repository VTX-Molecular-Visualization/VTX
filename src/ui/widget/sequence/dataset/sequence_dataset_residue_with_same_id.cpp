#include "sequence_dataset_residue_with_same_id.hpp"
#include "style.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	SequenceDisplayDataset_ResidueWithSameId::SequenceDisplayDataset_ResidueWithSameId( const Model::Chain & p_chain,
																						const uint p_startIndexChar,
																						const uint p_startResidueIndex,
																						const uint p_endResidueIndex ) :
		SequenceDisplayDataset( p_startIndexChar, p_endResidueIndex - p_startResidueIndex + 1 ),
		_startResidueIndex( p_startResidueIndex ), _endResidueIndex( p_endResidueIndex ), _linkedChain( p_chain ),
		_originalResidueIndex( p_chain.getMoleculePtr()
								   ->getResidue( p_chain.getIndexFirstResidue() + p_startResidueIndex )
								   ->getIndexInOriginalChain() ) {};

	void SequenceDisplayDataset_ResidueWithSameId::appendToSequence( QString & p_string ) const
	{
		const uint size			  = _endResidueIndex - _startResidueIndex + 1;
		QString	   sequenceString = QString();
		sequenceString.resize( size );

		const Model::Molecule * const molecule		  = _linkedChain.getMoleculePtr();
		const uint					  chainFirstIndex = _linkedChain.getIndexFirstResidue();

		for ( uint i = 0; i < size; i++ )
		{
			const Model::Residue * const residue = molecule->getResidue( chainFirstIndex + _startResidueIndex + i );
			sequenceString[ i ]					 = residue->getSymbolShort()[ 0 ];
		}

		p_string.append( sequenceString );
	}

	void SequenceDisplayDataset_ResidueWithSameId::appendToScale( QString &	 p_scale,
																  uint &	 p_lastIndexCharWritten,
																  const bool p_startBloc ) const
	{
		uint currentIndexChar;
		uint currentLocalIndexResidue;

		const std::string residueStr = std::to_string( _originalResidueIndex );

		if ( p_startBloc )
		{
			p_lastIndexCharWritten = _drawInScale( p_scale, residueStr, _startIndexChar, false );

			const uint nextValidCharIndex	  = p_lastIndexCharWritten + ( (uint)residueStr.size() / 2 ) + 2;
			const int  nextValidOriginalIndex = _originalResidueIndex + ( nextValidCharIndex - _startIndexChar );

			const uint step = _getStepToNextValidIndex( nextValidOriginalIndex );

			currentIndexChar		 = nextValidCharIndex + step;
			currentLocalIndexResidue = currentIndexChar - _startIndexChar;
		}
		else
		{
			const uint step = _getStepToNextValidIndex( _originalResidueIndex );

			currentIndexChar		 = _startIndexChar + step;
			currentLocalIndexResidue = step;

			const int charOffset = int( residueStr.size() ) / 2;

			if ( ( currentIndexChar - charOffset ) <= ( p_lastIndexCharWritten + 1 ) )
			{
				currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
				currentLocalIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP;
			}
		}

		uint endIndex = _endResidueIndex - _startResidueIndex;
		for ( ; currentLocalIndexResidue <= endIndex; currentLocalIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP )
		{
			p_lastIndexCharWritten = _drawInScale( p_scale, residueStr, currentIndexChar, true );
			currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
		}
	}

	Model::Residue * const SequenceDisplayDataset_ResidueWithSameId::getResidueAtCharIndex( const uint p_charIndex )
	{
		const uint firstIndexInOriginlaChain
			= _linkedChain.getMoleculePtr()
				  ->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex )
				  ->getIndex();
		uint residueIndex = firstIndexInOriginlaChain + p_charIndex - _startIndexChar;
		return _linkedChain.getMoleculePtr()->getResidue( residueIndex );
	}

	bool SequenceDisplayDataset_ResidueWithSameId::isResidueInScope( const uint p_residueIndex ) const
	{
		return _startResidueIndex <= p_residueIndex && p_residueIndex <= _endResidueIndex;
	}

	uint SequenceDisplayDataset_ResidueWithSameId::getCharIndexOfResidue( const uint p_residueIndex ) const
	{
		return _startIndexChar + p_residueIndex - _startResidueIndex;
	}

	uint SequenceDisplayDataset_ResidueWithSameId::getLastScaleCharIndex() const
	{
		const uint		  lastResidueIndex = getLastResidue()->getIndexInOriginalChain();
		const std::string lastIndexStr	   = std::to_string( lastResidueIndex );
		const uint lastIndexOffset = _charCount < Style::SEQUENCE_CHAIN_SCALE_STEP ? uint( lastIndexStr.length() )
																				   : uint( lastIndexStr.length() / 2 );

		return _charCount - ( lastResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) + lastIndexOffset;
	}

} // namespace VTX::UI::Widget::Sequence::Dataset
