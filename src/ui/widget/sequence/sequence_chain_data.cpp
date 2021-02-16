#include "sequence_chain_data.hpp"
#include "dataset/sequence_dataset_missing_residue.hpp"
#include "dataset/sequence_dataset_residue.hpp"
#include "dataset/sequence_dataset_tag.hpp"
#include "dataset/sequence_dataset_unknown_residue.hpp"

namespace VTX::UI::Widget::Sequence
{
	SequenceChainData::SequenceChainData( const Model::Chain & p_chain ) :
		_chain( p_chain ), _molecule( *( p_chain.getMoleculePtr() ) )
	{
		_generateDataSet();
		_generateString();
	};
	SequenceChainData ::~SequenceChainData()
	{
		for ( auto it : _dataset )
			delete it;

		_dataset.clear();
	}

	Model::Residue * const SequenceChainData::_getResidue( const uint p_localResidueIndex ) const
	{
		const uint moleculeResidueIndex = _chain.getIndexFirstResidue() + p_localResidueIndex;
		return _chain.getMoleculePtr()->getResidue( moleculeResidueIndex );
	}

	void SequenceChainData::_generateDataSet()
	{
		const uint residueCount = _chain.getResidueCount();

		Dataset::SequenceDisplayDataset_HtmlColorTag * const colorTag
			= new Dataset::SequenceDisplayDataset_HtmlColorTag( 0, _chain.getColor() );
		_dataset.emplace_back( colorTag );

		uint localCharIndex = 0;

		bool lastResidueWasUnknown				   = false;
		uint previousResidueScaleIndex			   = _getResidue( 0 )->getIndexInOriginalChain() - 1;
		uint sequentialResidueChainStartIndex	   = 0;
		bool startSequentialResidueChainStartIndex = true;

		for ( uint localResidueIndex = 0; localResidueIndex < residueCount; localResidueIndex++ )
		{
			Model::Residue * const residue = _getResidue( localResidueIndex );

			if ( residue == nullptr )
			{
				if ( localResidueIndex > 0 && startSequentialResidueChainStartIndex )
				{
					localCharIndex += _emplaceResidueDataSet(
						localCharIndex, sequentialResidueChainStartIndex, localResidueIndex - 1 );
				}

				const uint firstMissingResidueIndex = localResidueIndex;

				while ( _getResidue( localResidueIndex + 1 ) == nullptr )
				{
					localResidueIndex++;
				}

				const uint firstResidueIndexInOriginalChain = previousResidueScaleIndex + 1;
				const uint nbMissingResidue					= localResidueIndex - firstMissingResidueIndex + 1;

				Dataset::SequenceDisplayDataset_MissingResidue * missingResidueSet
					= new Dataset::SequenceDisplayDataset_MissingResidue(
						localCharIndex, firstResidueIndexInOriginalChain, nbMissingResidue );
				_dataset.emplace_back( missingResidueSet );
				localCharIndex += missingResidueSet->getStringSize();

				previousResidueScaleIndex += nbMissingResidue;
				startSequentialResidueChainStartIndex = false;
				lastResidueWasUnknown				  = false;

				continue;
			}

			uint residueScaleIndex = residue->getIndexInOriginalChain();

			bool	   isMissingResidue = residueScaleIndex != previousResidueScaleIndex + 1;
			const bool isUnknownResidue = residue->getSymbolShort() == "?" || residue->getSymbolShort() == "-";

			if ( isMissingResidue )
			{
				if ( localResidueIndex > 0 && startSequentialResidueChainStartIndex )
				{
					localCharIndex += _emplaceResidueDataSet(
						localCharIndex, sequentialResidueChainStartIndex, localResidueIndex - 1 );
				}

				const uint nbMissingResidue					= residueScaleIndex - previousResidueScaleIndex - 1;
				const uint firstResidueIndexInOriginalChain = previousResidueScaleIndex + 1;

				Dataset::SequenceDisplayDataset_MissingResidue * missingResidueSet
					= new Dataset::SequenceDisplayDataset_MissingResidue(
						localCharIndex, firstResidueIndexInOriginalChain, nbMissingResidue );
				_dataset.emplace_back( missingResidueSet );
				localCharIndex += missingResidueSet->getStringSize();

				startSequentialResidueChainStartIndex = false;
				lastResidueWasUnknown				  = false;
			}

			if ( isUnknownResidue )
			{
				if ( localResidueIndex > 0 && startSequentialResidueChainStartIndex )
				{
					localCharIndex += _emplaceResidueDataSet(
						localCharIndex, sequentialResidueChainStartIndex, localResidueIndex - 1 );
				}

				bool spaceBefore = localResidueIndex > 0 && !lastResidueWasUnknown;
				bool spaceAfter	 = localResidueIndex < ( residueCount - 1 );

				Dataset::SequenceDisplayDataset_UnknownResidue * unknownResidueSet
					= new Dataset::SequenceDisplayDataset_UnknownResidue(
						*residue, spaceBefore, spaceAfter, localCharIndex, residueScaleIndex );

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
			_emplaceResidueDataSet( localCharIndex, sequentialResidueChainStartIndex, residueCount - 1 );
		}

		_dataset.emplace_back( new Dataset::SequenceDisplayDataset_EndHtmlColorTag( localCharIndex ) );
	};

	uint SequenceChainData::_emplaceResidueDataSet( const uint p_localCharIndex,
													const uint p_startResidueIndex,
													const uint p_endResidueIndex )
	{
		Dataset::SequenceDisplayDataset_Residue * residueSet = new Dataset::SequenceDisplayDataset_Residue(
			_chain, p_localCharIndex, p_startResidueIndex, p_endResidueIndex );
		_dataset.emplace_back( residueSet );

		return residueSet->getStringSize();
	}

	void SequenceChainData::_generateString()
	{
		uint sequenceLength = 0;
		_strSequence		= QString();

		for ( Dataset::SequenceDisplayDataset * const it : _dataset )
		{
			it->appendToSequence( _strSequence );
			sequenceLength += it->getStringSize();
		}

		const uint firstResidueIndex
			= _chain.getMoleculePtr()->getResidue( _chain.getIndexFirstResidue() )->getIndexInOriginalChain();
		const uint lastResidueIndex
			= _chain.getMoleculePtr()->getResidue( _chain.getIndexLastResidue() )->getIndexInOriginalChain();

		const std::string firstResidueIndexStr = std::to_string( firstResidueIndex );
		const std::string lastResidueIndexStr  = std::to_string( lastResidueIndex );

		const uint lastResidueOffset
			= ( uint )( sequenceLength < Style::SEQUENCE_CHAIN_SCALE_STEP ? lastResidueIndexStr.size()
																		  : ( lastResidueIndexStr.size() / 2 ) );
		const uint scaleLength
			= sequenceLength - ( lastResidueIndex % Style::SEQUENCE_CHAIN_SCALE_STEP ) + lastResidueOffset;

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
	Model::Residue * const SequenceChainData::getClosestResidueFromCharIndex( const uint p_charIndex,
																			  const bool takeForward ) const
	{
		Model::Residue * res = nullptr;

		bool takeNext = false;

		for ( const auto it : _dataset )
		{
			if ( takeNext )
			{
				res = it->getFirstResidue();
				break;
			}

			if ( it->isCharIndexInScope( p_charIndex ) )
			{
				if ( takeForward )
					takeNext = true;
				else
					break; // res set to previousIt->getLastResidue.
			}
			res = it->getLastResidue();
		}

		return res;
	}

	uint SequenceChainData::getCharIndex( const uint p_residueIndex ) const
	{
		Dataset::SequenceDisplayDataset * const data = getDataset( p_residueIndex );

		if ( data != nullptr )
			return data->getCharIndexOfResidue( p_residueIndex );
		else
			return -1;
	}

	uint SequenceChainData::getPaintCharIndex( const uint p_residueIndex ) const
	{
		Dataset::SequenceDisplayDataset * const data = getDataset( p_residueIndex );

		if ( data != nullptr )
			return data->getPaintCharIndex( p_residueIndex );
		else
			return -1;
	}
	uint SequenceChainData::getPaintLength( const uint p_residueIndex ) const
	{
		Dataset::SequenceDisplayDataset * const data = getDataset( p_residueIndex );

		if ( data != nullptr )
			return data->getPaintLength( p_residueIndex );
		else
			return 0;
	}

	Dataset::SequenceDisplayDataset * const SequenceChainData::getDataset( const uint p_residueIndex ) const
	{
		for ( const auto it : _dataset )
		{
			if ( it->isResidueInScope( p_residueIndex ) )
				return it;
		}

		return nullptr;
	}

	Dataset::SequenceDisplayDataset * const SequenceChainData::getDataset_recursive(
		const std::vector<Dataset::SequenceDisplayDataset *> p_vec,
		const uint											 p_residueIndex,
		const uint											 p_indexMin,
		const uint											 p_indexMax,
		const bool											 p_minHasChanged ) const
	{
		if ( p_indexMax <= p_indexMin )
		{
			Dataset::SequenceDisplayDataset * const middleObj
				= p_minHasChanged ? p_vec[ p_indexMin ] : p_vec[ p_indexMax ];

			if ( middleObj->getFirstResidue()->getIndex() <= p_residueIndex
				 && p_residueIndex <= middleObj->getLastResidue()->getIndex() )
				return middleObj;
			else
				return nullptr;
		}

		const uint								middleIndex = p_indexMin + ( ( p_indexMax - p_indexMin ) / 2 );
		Dataset::SequenceDisplayDataset * const middleObj	= p_vec[ middleIndex ];

		if ( middleObj->getFirstResidue() != nullptr )
		{
			if ( p_minHasChanged )
				return getDataset_recursive( p_vec, p_residueIndex, p_indexMin + 1, p_indexMax, true );
			else
				return getDataset_recursive( p_vec, p_residueIndex, p_indexMin, p_indexMax - 1, false );
		}
		else if ( middleObj->getFirstResidue()->getIndex() < p_residueIndex )
			return getDataset_recursive( p_vec, p_residueIndex, p_indexMin, middleIndex - 1, false );
		else if ( middleObj->getLastResidue()->getIndex() > p_residueIndex )
			return getDataset_recursive( p_vec, p_residueIndex, middleIndex + 1, p_indexMax, true );
		else
			return middleObj;
	}
} // namespace VTX::UI::Widget::Sequence
