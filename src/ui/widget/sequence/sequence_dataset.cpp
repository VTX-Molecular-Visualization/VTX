#include "sequence_dataset.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				SequenceDisplayDataset_Residue::SequenceDisplayDataset_Residue( const Model::Chain & p_chain,
																				const uint			 p_startIndexChar,
																				const uint			 p_startResidueIndex,
																				const uint			 p_endResidueIndex ) :
					SequenceDisplayDataset( p_startIndexChar, p_startIndexChar + ( p_endResidueIndex - p_startResidueIndex ) ),
					_startResidueIndex( p_startResidueIndex ), _endResidueIndex( p_endResidueIndex ), _linkedChain( p_chain )
				{
					const uint size = p_endResidueIndex - p_startResidueIndex + 1;
					_str			= QString( size, ' ' );

					const Model::Molecule * const molecule		  = p_chain.getMoleculePtr();
					const uint					  chainFirstIndex = p_chain.getIndexFirstResidue();

					for ( uint i = 0; i < size; i++ )
						_str[ i ] = molecule->getResidue( chainFirstIndex + p_startResidueIndex + i ).getSymbolShort()[ 0 ];
				};

				const void SequenceDisplayDataset_Residue::appendToScale( QString & p_scale, const bool p_startBloc ) const
				{
					uint currentIndexChar;
					uint currentLocalIndexResidue;

					const uint firstResidue = _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _startResidueIndex ).getIndexInOriginalChain();

					if ( p_startBloc )
					{
						const std::string firstResidueStr	 = std::to_string( firstResidue );
						const uint		  lastCharFirstIndex = _drawInScale( p_scale, firstResidueStr, _startIndexChar, false );

						const std::string strSecondIndex = std::to_string( firstResidue + 1 );

						const uint nextValidIndex = lastCharFirstIndex + ( (uint)strSecondIndex.size() / 2 ) + 1;

						const uint residueNextValidIndex = firstResidue + ( nextValidIndex - _startIndexChar );

						const uint moduloStep = residueNextValidIndex % Style::SEQUENCE_CHAIN_SCALE_STEP;
						const uint step		  = moduloStep == 0 ? 0 : ( Style::SEQUENCE_CHAIN_SCALE_STEP - moduloStep );

						currentIndexChar		 = nextValidIndex + step;
						currentLocalIndexResidue = _startResidueIndex + ( currentIndexChar - _startIndexChar );
					}
					else
					{
						uint moduloStep = firstResidue % Style::SEQUENCE_CHAIN_SCALE_STEP;
						uint step		= moduloStep == 0 ? 0 : Style::SEQUENCE_CHAIN_SCALE_STEP - ( firstResidue % Style::SEQUENCE_CHAIN_SCALE_STEP );

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

					_endIndexChar = p_startIndexChar + _str.size() - 1;
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

				SequenceDisplayDataset_HtmlColorTag::SequenceDisplayDataset_HtmlColorTag( const uint p_charIndex, const Color::Rgb & p_color ) :
					SequenceDisplayDataset_HtmlTag( p_charIndex )
				{
					_str = QString();
					Util::UI::appendColorHtmlTag( _str, p_color );
				};

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
