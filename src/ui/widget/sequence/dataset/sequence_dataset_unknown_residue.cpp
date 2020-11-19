#include "sequence_dataset_unknown_residue.hpp"
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
					SequenceDisplayDataset_UnknownResidue::SequenceDisplayDataset_UnknownResidue( Model::Residue & p_residue,
																								  const bool	   p_spaceBefore,
																								  const bool	   p_spaceAfter,
																								  const uint	   p_startIndexChar,
																								  const uint	   p_residueIndex ) :
						SequenceDisplayDataset( p_startIndexChar, p_startIndexChar ),
						_residueIndex( p_residueIndex ), _residue( &p_residue )
					{
						_str = QString();

						if ( p_spaceBefore )
							_str.append( ' ' );

						const std::string & symbol = p_residue.getSymbolStr();
						_str.append( QString::fromStdString( symbol ) );

						_scaleIndexPosition = ( uint )( _str.length() - ( ( symbol.length() + 1 ) / 2 ) );

						if ( p_spaceAfter )
							_str.append( ' ' );

						_charCount	  = _str.size();
						_endIndexChar = p_startIndexChar + _charCount - 1;
					}
					const void SequenceDisplayDataset_UnknownResidue::appendToScale( QString & p_scale, bool p_startBloc ) const
					{
						if ( p_startBloc || ( ( _residueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) == 0 ) )
						{
							const std::string residueIndexStr = std::to_string( _residueIndex );
							_drawInScale( p_scale, residueIndexStr, _startIndexChar + _scaleIndexPosition, true );
						}
					}

					Model::Residue * const SequenceDisplayDataset_UnknownResidue::getResidueAtCharIndex( const uint p_charIndex ) { return _residue; }

				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
