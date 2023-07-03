#include "ui/old_ui/ui/widget/sequence/dataset/sequence_dataset_missing_residue.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	void SequenceDisplayDataset_MissingResidue::appendToSequence( QString & p_sequenceString ) const
	{
		if ( _isTooLong )
		{
			p_sequenceString.append( TOO_LONG_STR );
		}
		else
		{
			p_sequenceString.append( QString( _charCount, '-' ) );
		}
	}

	void SequenceDisplayDataset_MissingResidue::appendToScale( QString &  p_scale,
															   uint &	  p_lastIndexCharWritten,
															   const bool p_startBloc ) const
	{
		if ( _isTooLong )
			return;

		uint currentIndexChar;
		int	 currentIndexResidue;

		const int firstResidue = _startResidueIndexInOriginalChain;
		const int endResidue   = _startResidueIndexInOriginalChain + _charCount;

		if ( p_startBloc )
		{
			const std::string firstResidueStr = std::to_string( firstResidue );
			p_lastIndexCharWritten			  = _drawInScale( p_scale, firstResidueStr, _startIndexChar, false );

			const std::string strSecondIndex = std::to_string( firstResidue + 1 );

			const uint nextValidIndex = p_lastIndexCharWritten + ( (uint)strSecondIndex.size() / 2 ) + 2;

			currentIndexChar
				= nextValidIndex + ( ( firstResidue + nextValidIndex ) % Style::SEQUENCE_CHAIN_SCALE_STEP );
			currentIndexResidue = firstResidue + currentIndexChar - _startIndexChar;
		}
		else
		{
			const uint step = _getStepToNextValidIndex( firstResidue );

			currentIndexChar	= _startIndexChar + step;
			currentIndexResidue = firstResidue + step;

			const int charOffset
				= int( std::to_string( _startResidueIndexInOriginalChain + currentIndexResidue ).size() ) / 2;

			if ( ( currentIndexChar - charOffset ) <= ( p_lastIndexCharWritten + 1 ) )
			{
				currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
				currentIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP;
			}
		}

		for ( ; currentIndexResidue <= endResidue; currentIndexResidue += Style::SEQUENCE_CHAIN_SCALE_STEP )
		{
			std::string strIndex   = std::to_string( currentIndexResidue );
			p_lastIndexCharWritten = _drawInScale( p_scale, strIndex, currentIndexChar, true );

			currentIndexChar += Style::SEQUENCE_CHAIN_SCALE_STEP;
		}
	}

	uint SequenceDisplayDataset_MissingResidue::getLastScaleCharIndex() const
	{
		if ( _isTooLong )
			return 0;

		const uint		  lastResidueIndex = getLastResidue()->getIndexInOriginalChain();
		const std::string lastIndexStr	   = std::to_string( lastResidueIndex );
		const uint lastIndexOffset = _charCount < Style::SEQUENCE_CHAIN_SCALE_STEP ? uint( lastIndexStr.length() )
																				   : uint( lastIndexStr.length() / 2 );

		return _charCount - ( lastResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) + lastIndexOffset;
	}

} // namespace VTX::UI::Widget::Sequence::Dataset
