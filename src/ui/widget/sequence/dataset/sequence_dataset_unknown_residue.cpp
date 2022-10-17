#include "sequence_dataset_unknown_residue.hpp"
#include "style.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	SequenceDisplayDataset_UnknownResidue::SequenceDisplayDataset_UnknownResidue( Model::Residue & p_residue,
																				  const bool	   p_spaceBefore,
																				  const bool	   p_spaceAfter,
																				  const uint	   p_startIndexChar,
																				  const uint	   p_residueIndex ) :
		SequenceDisplayDataset( p_startIndexChar, (uint)p_residue.getSymbolStr().size() ),
		_residueIndex( p_residueIndex ), _residue( &p_residue ), _spaceBefore( p_spaceBefore ),
		_spaceAfter( p_spaceAfter )
	{
		if ( p_spaceBefore )
			_charCount++;

		if ( p_spaceAfter )
			_charCount++;
	}

	void SequenceDisplayDataset_UnknownResidue::appendToSequence( QString & p_sequenceString ) const
	{
		QString str = QString();

		if ( _spaceBefore )
			str.append( ' ' );

		const std::string & symbol = _residue->getSymbolStr();
		str.append( QString::fromStdString( symbol ) );

		if ( _spaceAfter )
			str.append( ' ' );

		p_sequenceString.append( str );
	}

	void SequenceDisplayDataset_UnknownResidue::appendToScale( QString & p_scale,
															   uint &	 p_lastIndexCharWritten,
															   bool		 p_startBloc ) const
	{
		const bool		  indexInitialized = _residueIndex != (uint)INT_MIN;
		const std::string residueIndexStr  = indexInitialized ? std::to_string( _residueIndex ) : "?";

		const bool center		   = !p_startBloc;
		uint	   charIndexOffset = center ? uint( ( _residue->getSymbolStr().length() - 1 ) / 2 ) : 0;
		charIndexOffset += _spaceBefore ? 1 : 0;

		const uint firstIndexPosition = _startIndexChar + charIndexOffset - ( int( residueIndexStr.size() ) / 2 );

		if ( p_startBloc || ( firstIndexPosition > ( p_lastIndexCharWritten + 1 ) ) )
		{
			p_lastIndexCharWritten
				= _drawInScale( p_scale, residueIndexStr, _startIndexChar + charIndexOffset, center );
		}
	}

	Model::Residue * const SequenceDisplayDataset_UnknownResidue::getResidueAtCharIndex( const uint p_charIndex )
	{
		return _residue;
	}

	uint SequenceDisplayDataset_UnknownResidue::getLastScaleCharIndex() const
	{
		const uint residueSize = uint( std::to_string( _residue->getIndexInOriginalChain() ).size() );
		const uint offset	   = _spaceBefore ? 1 : 0;

		return residueSize + offset;
	}

} // namespace VTX::UI::Widget::Sequence::Dataset
