#include "ui/old_ui/ui/widget/sequence/sequence_chain_data.hpp"
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>

namespace VTX::UI::Widget::Sequence
{
	SequenceChainData::SequenceChainData( const App::Component::Chemistry::Chain & p_chain ) :
		_chain( p_chain ), _molecule( *( p_chain.getMoleculePtr() ) ), _fromResidue( _chain.getIndexFirstResidue() ),
		_toResidue( _chain.getIndexLastResidue() ), _chainSequenceBuilder( p_chain )
	{
		_chainSequenceBuilder.generate();
		_generateString();
	};
	SequenceChainData ::~SequenceChainData() {}

	void SequenceChainData::_generateString()
	{
		uint sequenceLength = 0;
		_strSequence		= QString();

		for ( Dataset::SequenceDisplayDataset * const it : _chainSequenceBuilder.getDataSet() )
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

		const uint lastResidueOffset = uint( lastResidueIndexStr.size() - 1 );

		// Take last relevent data
		auto it = _chainSequenceBuilder.getDataSet().crbegin();
		while ( it != _chainSequenceBuilder.getDataSet().crend() && ( *it )->getStringSize() == 0 )
		{
			it++;
		}

		const int scaleOffset = ( *it )->getLastScaleCharIndex() - ( *it )->getStringSize();

		const uint scaleLength = scaleOffset > 0 ? sequenceLength + scaleOffset : sequenceLength;

		_strScale = QString( scaleLength, ' ' );

		bool startBlock			  = true;
		uint lastIndexCharWritten = 0;
		for ( auto it : _chainSequenceBuilder.getDataSet() )
		{
			it->appendToScale( _strScale, lastIndexCharWritten, startBlock );
			startBlock = it->isFinishingBlock( startBlock );
		}
	}

	App::Component::Chemistry::Residue * const SequenceChainData::getResidueFromCharIndex(
		const uint p_charIndex ) const
	{
		for ( const auto it : _chainSequenceBuilder.getDataSet() )
		{
			if ( it->isCharIndexInScope( p_charIndex ) )
				return it->getResidueAtCharIndex( p_charIndex );
		}

		return nullptr;
	}
	App::Component::Chemistry::Residue * const SequenceChainData::getClosestResidueFromCharIndex(
		const uint p_charIndex,
		const bool takeForward ) const
	{
		App::Component::Chemistry::Residue * res = nullptr;

		bool takeNext = false;

		for ( const auto it : _chainSequenceBuilder.getDataSet() )
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
		for ( const auto it : _chainSequenceBuilder.getDataSet() )
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
