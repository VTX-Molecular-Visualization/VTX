#include "sequence_chain_data.hpp"
#include "dataset/sequence_dataset_missing_residue.hpp"
#include "dataset/sequence_dataset_residue.hpp"
#include "dataset/sequence_dataset_tag.hpp"
#include "dataset/sequence_dataset_unknown_residue.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				void SequenceChainData::_generateDataSet()
				{
					const uint residueCount = _chain.getResidueCount();

					Dataset::SequenceDisplayDataset_HtmlColorTag * const colorTag = new Dataset::SequenceDisplayDataset_HtmlColorTag( 0, _chain.getColor() );
					_dataset.emplace_back( colorTag );

					uint localCharIndex = 0;

					bool lastResidueWasUnknown				   = false;
					uint previousResidueScaleIndex			   = _getResidue( 0 ).getIndexInOriginalChain() - 1;
					uint sequentialResidueChainStartIndex	   = 0;
					bool startSequentialResidueChainStartIndex = true;

					for ( uint localResidueIndex = 0; localResidueIndex < residueCount; localResidueIndex++ )
					{
						Model::Residue & residue		   = _getResidue( localResidueIndex );
						uint			 residueScaleIndex = residue.getIndexInOriginalChain();

						bool	   isMissingResidue = residueScaleIndex != previousResidueScaleIndex + 1;
						const bool isUnknownResidue = residue.getSymbolShort() == "?" || residue.getSymbolShort() == "-";

						if ( isMissingResidue )
						{
							if ( localResidueIndex > 0 && startSequentialResidueChainStartIndex )
							{
								Dataset::SequenceDisplayDataset_Residue * residueSet
									= new Dataset::SequenceDisplayDataset_Residue( _chain, localCharIndex, sequentialResidueChainStartIndex, localResidueIndex - 1 );
								_dataset.emplace_back( residueSet );

								uint stringSize = residueSet->getStringSize();
								localCharIndex += stringSize;
							}

							const uint nbMissingResidue					= residueScaleIndex - previousResidueScaleIndex - 1;
							const uint firstResidueIndexInOriginalChain = previousResidueScaleIndex + 1;

							Dataset::SequenceDisplayDataset_MissingResidue * missingResidueSet
								= new Dataset::SequenceDisplayDataset_MissingResidue( localCharIndex, firstResidueIndexInOriginalChain, nbMissingResidue );
							_dataset.emplace_back( missingResidueSet );
							localCharIndex += missingResidueSet->getStringSize();

							startSequentialResidueChainStartIndex = false;
							lastResidueWasUnknown				  = false;
						}

						if ( isUnknownResidue )
						{
							if ( localResidueIndex > 0 && startSequentialResidueChainStartIndex )
							{
								Dataset::SequenceDisplayDataset_Residue * residueSet
									= new Dataset::SequenceDisplayDataset_Residue( _chain, localCharIndex, sequentialResidueChainStartIndex, localResidueIndex - 1 );
								_dataset.emplace_back( residueSet );

								localCharIndex += residueSet->getStringSize();
							}

							bool spaceBefore = localResidueIndex > 0 && !lastResidueWasUnknown;
							bool spaceAfter	 = localResidueIndex < ( residueCount - 1 );

							Dataset::SequenceDisplayDataset_UnknownResidue * unknownResidueSet
								= new Dataset::SequenceDisplayDataset_UnknownResidue( residue, spaceBefore, spaceAfter, localCharIndex, residueScaleIndex );

							_dataset.emplace_back( unknownResidueSet );
							localCharIndex += unknownResidueSet->getStringSize();

							startSequentialResidueChainStartIndex = false;
							lastResidueWasUnknown				  = true;
						}
						else
						{
							if ( !startSequentialResidueChainStartIndex )
							{
								sequentialResidueChainStartIndex	  = localResidueIndex;
								startSequentialResidueChainStartIndex = true;
							}
						}

						previousResidueScaleIndex = residueScaleIndex;
					}

					if ( startSequentialResidueChainStartIndex )
					{
						Dataset::SequenceDisplayDataset_Residue * residueSet
							= new Dataset::SequenceDisplayDataset_Residue( _chain, localCharIndex, sequentialResidueChainStartIndex, residueCount - 1 );
						_dataset.emplace_back( residueSet );

						localCharIndex += residueSet->getStringSize();
						startSequentialResidueChainStartIndex = false;
					}

					_dataset.emplace_back( new Dataset::SequenceDisplayDataset_EndHtmlColorTag( localCharIndex ) );
				};

				void SequenceChainData::_generateString()
				{
					uint sequenceLength = 0;
					_strSequence		= QString();

					for ( auto it : _dataset )
					{
						_strSequence.append( it->getString() );
						sequenceLength += it->getStringSize();
					}
					const uint firstResidueIndex = _chain.getMoleculePtr()->getResidue( _chain.getIndexFirstResidue() ).getIndexInOriginalChain();
					const uint lastResidueIndex	 = _chain.getMoleculePtr()->getResidue( _chain.getIndexLastResidue() ).getIndexInOriginalChain();

					const std::string firstResidueIndexStr = std::to_string( firstResidueIndex );
					const std::string lastResidueIndexStr  = std::to_string( lastResidueIndex );

					const uint scaleLength = sequenceLength - ( lastResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) + ( (uint)lastResidueIndexStr.size() / 2 );

					_strScale = QString( scaleLength, ' ' );

					bool startBlock = true;

					for ( auto it : _dataset )
					{
						it->appendToScale( _strScale, startBlock );
						startBlock = it->isFinishingBlock( startBlock );
					}
				}

				Model::Residue * const SequenceChainData::getResidueFromCharIndex( const uint p_charIndex ) const
				{
					Model::Residue * res = nullptr;

					for ( const auto it : _dataset )
					{
						if ( it->isCharIndexInScope( p_charIndex ) )
							return it->getResidueAtCharIndex( p_charIndex );
					}

					return nullptr;
				}
				Model::Residue * const SequenceChainData::getClosestResidueFromCharIndex( const uint p_charIndex, const bool takeForward ) const
				{
					Model::Residue * res = nullptr;

					bool takeNext = false;

					for ( const auto it : _dataset )
					{
						if ( takeNext )
						{
							res = it->getFirstResidue( p_charIndex );
							break;
						}

						if ( it->isCharIndexInScope( p_charIndex ) )
						{
							if ( takeForward )
								takeNext = true;
							else
								break; // res set to previousIt->getLastResidue.
						}
						res = it->getLastResidue( p_charIndex );
					}

					return res;
				}

				uint SequenceChainData::getCharIndex( const uint p_residueIndex ) const
				{
					for ( const auto it : _dataset )
					{
						if ( it->isResidueInScope( p_residueIndex ) )
							return it->getCharIndexOfResidue( p_residueIndex );
					}

					return -1;
				}

				uint SequenceChainData::getPaintCharIndex( const uint p_residueIndex ) const
				{
					for ( const auto it : _dataset )
					{
						if ( it->isResidueInScope( p_residueIndex ) )
							return it->getPaintCharIndex( p_residueIndex );
					}

					return -1;
				}
				uint SequenceChainData::getPaintLength( const uint p_residueIndex ) const
				{
					for ( const auto it : _dataset )
					{
						if ( it->isResidueInScope( p_residueIndex ) )
							return it->getPaintLength( p_residueIndex );
					}

					return 0;
				}

			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
