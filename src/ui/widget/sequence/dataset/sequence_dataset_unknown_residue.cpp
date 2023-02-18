#include "sequence_dataset_unknown_residue.hpp"
#include "model/residue.hpp"
#include "style.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	SequenceDisplayDataset_UnknownResidue::SequenceDisplayDataset_UnknownResidue( const Model::Chain & p_chain,
																				  const uint		   p_startIndexChar,
																				  const uint p_residueFirstIndex,
																				  const uint p_residueLastIndex,
																				  const bool p_spaceBefore,
																				  const bool p_spaceAfter ) :
		SequenceDisplayDataset( p_startIndexChar, 0 ),
		_residueFirstIndex( p_residueFirstIndex ), _residueLastIndex( p_residueLastIndex ), _chain( &p_chain ),
		_spaceBefore( p_spaceBefore ), _spaceAfter( p_spaceAfter )
	{
		_buildInnerData();
	}

	void SequenceDisplayDataset_UnknownResidue::_buildInnerData()
	{
		_charCount						= 0;
		_charLengthCount				= std::vector<PairCharLengthCount>();
		uint currentCharLength			= 0;
		uint firstResidueOfCurrentCount = _residueFirstIndex;

		const Model::Molecule * const molecule = _chain->getMoleculePtr();

		for ( uint i = _residueFirstIndex; i <= _residueLastIndex; i++ )
		{
			const Model::Residue * const residue = molecule->getResidue( _chain->getIndexFirstResidue() + i );
			const uint					 residueSymbolStrLength = uint( residue->getSymbolStr().length() );

			if ( residueSymbolStrLength != currentCharLength )
			{
				const uint residueCount = i - firstResidueOfCurrentCount;

				if ( residueCount > 0 )
				{
					_charLengthCount.emplace_back( PairCharLengthCount( currentCharLength, residueCount ) );
				}

				firstResidueOfCurrentCount = i;
				currentCharLength		   = residueSymbolStrLength;
			}
			_charCount += residueSymbolStrLength;
		}

		const uint residueCount = _residueLastIndex - firstResidueOfCurrentCount + 1;

		if ( residueCount > 0 )
		{
			_charLengthCount.emplace_back( PairCharLengthCount( currentCharLength, residueCount ) );
		}

		// Add spaces
		_charCount += ( _residueLastIndex - _residueFirstIndex + 1 );

		// Add or remove extra spaces linked to space before and after (by default, one extra space is computed by the
		// previous method)
		if ( _spaceBefore && _spaceAfter )
			_charCount++;
		else if ( !_spaceBefore && !_spaceAfter )
			_charCount--;
	}

	void SequenceDisplayDataset_UnknownResidue::appendToSequence( QString & p_sequenceString ) const
	{
		QString str = QString();

		if ( _spaceBefore )
			str.append( ' ' );

		const Model::Molecule & p_molecule = *_chain->getMoleculePtr();

		const uint residueFirstIndexInMolecule = _chain->getIndexFirstResidue() + _residueFirstIndex;
		const uint residueLastIndexInMolecule  = _chain->getIndexFirstResidue() + _residueLastIndex;
		for ( uint i = residueFirstIndexInMolecule; i < residueLastIndexInMolecule; i++ )
		{
			const Model::Residue * const residue = p_molecule.getResidue( i );
			const std::string &			 symbol	 = residue->getSymbolStr();

			str.append( QString::fromStdString( symbol ) );
			str.append( ' ' );
		}

		const Model::Residue * const residue = p_molecule.getResidue( residueLastIndexInMolecule );
		const std::string &			 symbol	 = residue->getSymbolStr();

		str.append( QString::fromStdString( symbol ) );

		if ( _spaceAfter )
			str.append( ' ' );

		p_sequenceString.append( str );
	}

	void SequenceDisplayDataset_UnknownResidue::appendToScale( QString & p_scale,
															   uint &	 p_lastIndexCharWritten,
															   bool		 p_startBloc ) const
	{
		const Model::Molecule & molecule = *_chain->getMoleculePtr();
		const Model::Residue *	residue	 = molecule.getResidue( _chain->getIndexFirstResidue() + _residueFirstIndex );
		uint					residueSymbolStrLength = uint( residue->getSymbolStr().length() );
		std::string				residueIndexStr		   = std::to_string( residue->getIndexInOriginalChain() );

		uint currentCharOffset = _startIndexChar + ( _spaceBefore ? 1 : 0 );

		if ( p_startBloc )
		{
			uint localCharIndexOffset = uint( ( residueSymbolStrLength - 1 ) / 2 );
			p_lastIndexCharWritten
				= _drawInScale( p_scale, residueIndexStr, currentCharOffset + localCharIndexOffset, true );
		}
		else
		{
			uint	   localCharIndexOffset = uint( ( residueSymbolStrLength - 1 ) / 2 );
			const uint firstIndexPosition	= currentCharOffset - ( int( residueIndexStr.size() ) / 2 );
			if ( ( firstIndexPosition > ( p_lastIndexCharWritten + 1 ) ) )
			{
				p_lastIndexCharWritten
					= _drawInScale( p_scale, residueIndexStr, currentCharOffset + localCharIndexOffset, true );
			}
		}

		currentCharOffset += residueSymbolStrLength + 1;

		for ( uint i = _residueFirstIndex + 1; i <= _residueLastIndex; i++ )
		{
			residue							= molecule.getResidue( _chain->getIndexFirstResidue() + i );
			residueSymbolStrLength			= uint( residue->getSymbolStr().length() );
			residueIndexStr					= std::to_string( residue->getIndexInOriginalChain() );
			uint	   localCharIndexOffset = uint( ( residueSymbolStrLength - 1 ) / 2 );
			const uint firstIndexPosition	= currentCharOffset - ( int( residueIndexStr.size() ) / 2 );

			if ( firstIndexPosition > ( p_lastIndexCharWritten + 1 ) )
			{
				p_lastIndexCharWritten
					= _drawInScale( p_scale, residueIndexStr, currentCharOffset + localCharIndexOffset, true );
			}

			currentCharOffset += ( residueSymbolStrLength + 1 );
		}
	}

	uint SequenceDisplayDataset_UnknownResidue::getPaintLength( const uint p_residueIndex ) const
	{
		const uint residueIndex = _chain->getIndexFirstResidue() + p_residueIndex;
		return uint( _chain->getMoleculePtr()->getResidue( residueIndex )->getSymbolStr().length() );
	};

	uint SequenceDisplayDataset_UnknownResidue::getPaintCharIndex( const uint p_residueIndex ) const
	{
		uint charOffset			 = _startIndexChar + ( _spaceBefore ? 1 : 0 );
		uint currentResidueIndex = _residueFirstIndex;

		for ( const PairCharLengthCount & pair : _charLengthCount )
		{
			if ( currentResidueIndex + pair.second < p_residueIndex )
			{
				currentResidueIndex += pair.second;
				charOffset += ( pair.first + 1 ) * pair.second;
			}
			else
			{
				const uint residueOffset = ( p_residueIndex - currentResidueIndex );
				charOffset += ( pair.first + 1 ) * residueOffset;
				break;
			}
		}

		return charOffset;
	};

	Model::Residue * const SequenceDisplayDataset_UnknownResidue::getResidueAtCharIndex( const uint p_charIndex )
	{
		uint charOffset	  = _startIndexChar + ( _spaceBefore ? 1 : 0 );
		uint residueIndex = _residueFirstIndex;

		for ( const PairCharLengthCount & pair : _charLengthCount )
		{
			const uint charLengthPair = ( pair.first + 1 ) * pair.second;
			if ( charOffset + charLengthPair < p_charIndex )
			{
				charOffset += charLengthPair;
				residueIndex += pair.second;
			}
			else
			{
				const uint charOffsetInLastPair = ( p_charIndex - charOffset );
				residueIndex += charOffsetInLastPair / ( pair.first + 1 );
				break;
			}
		}

		return _chain->getMoleculePtr()->getResidue( _chain->getIndexFirstResidue() + residueIndex );
	}

	uint SequenceDisplayDataset_UnknownResidue::getLastScaleCharIndex() const
	{
		uint res = _charCount;
		res -= _spaceAfter ? 1 : 0;
		const Model::Residue * const lastResidue
			= _chain->getMoleculePtr()->getResidue( _chain->getIndexFirstResidue() + _residueLastIndex );
		const uint lastIndexStrLength = uint( std::to_string( lastResidue->getIndexInOriginalChain() ).size() );

		res -= lastIndexStrLength / 2;

		return res;
	}

} // namespace VTX::UI::Widget::Sequence::Dataset
