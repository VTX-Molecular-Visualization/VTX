#include "ui/old_ui/ui/widget/sequence/chain_sequence_builder.hpp"
#include "ui/old_ui/ui/widget/sequence/dataset/sequence_dataset_missing_residue.hpp"
#include "ui/old_ui/ui/widget/sequence/dataset/sequence_dataset_residue.hpp"
#include "ui/old_ui/ui/widget/sequence/dataset/sequence_dataset_tag.hpp"
#include "ui/old_ui/ui/widget/sequence/dataset/sequence_dataset_unknown_residue.hpp"

namespace VTX::UI::Widget::Sequence
{
	ChainSequenceBuilder::ChainSequenceBuilder( const Model::Chain & p_chain ) : _chain( p_chain ) {};
	ChainSequenceBuilder ::~ChainSequenceBuilder() { clear(); }

	void ChainSequenceBuilder::generate()
	{
		const uint residueCount = _chain.getResidueCount();

		Dataset::SequenceDisplayDataset_HtmlColorTag * const colorTag
			= new Dataset::SequenceDisplayDataset_HtmlColorTag( 0, _chain.getColor() );
		_dataset.emplace_back( colorTag );

		_currentCharPos						   = 0;
		BuildingDatasetMetadata currentDataset = BuildingDatasetMetadata();

		uint previousResidueScaleIndex			   = _getResidue( 0 )->getIndexInOriginalChain() - 1;
		uint sequentialResidueChainStartIndex	   = 0;
		bool startSequentialResidueChainStartIndex = true;

		for ( uint localResidueIndex = 0; localResidueIndex < residueCount; localResidueIndex++ )
		{
			Model::Residue * const residue = _getResidue( localResidueIndex );

			if ( residue == nullptr )
			{
				currentDataset.lastResidueIndex = localResidueIndex - 1;
				_emplaceDataSet( currentDataset );

				currentDataset					 = BuildingDatasetMetadata( DatasetType::MISSING );
				currentDataset.firstResidueIndex = previousResidueScaleIndex + 1;

				uint startResidueIndex = localResidueIndex;

				while ( _getResidue( localResidueIndex + 1 ) == nullptr )
				{
					localResidueIndex++;
				}

				const uint missingResidueCount = localResidueIndex - startResidueIndex;

				currentDataset.lastResidueIndex = currentDataset.firstResidueIndex + missingResidueCount;
				_emplaceDataSet( currentDataset );

				previousResidueScaleIndex += missingResidueCount;

				currentDataset = BuildingDatasetMetadata( DatasetType::NOT_SET );

				continue;
			}

			uint residueScaleIndex = residue->getIndexInOriginalChain();

			const bool residueAreContiguous
				= residueScaleIndex == previousResidueScaleIndex + 1 || residueScaleIndex == previousResidueScaleIndex;
			if ( !residueAreContiguous )
			{
				currentDataset.lastResidueIndex = localResidueIndex - 1;
				_emplaceDataSet( currentDataset );

				currentDataset					 = BuildingDatasetMetadata( DatasetType::MISSING );
				currentDataset.firstResidueIndex = previousResidueScaleIndex + 1;
				currentDataset.lastResidueIndex	 = residueScaleIndex - 1;
				_emplaceDataSet( currentDataset );

				currentDataset = BuildingDatasetMetadata( DatasetType::NOT_SET );
			}

			const bool isUnknownResidue = residue->getSymbolShort() == "?" || residue->getSymbolShort() == "-";
			if ( isUnknownResidue )
			{
				if ( currentDataset.type != DatasetType::UNKNOWN )
				{
					currentDataset.lastResidueIndex = localResidueIndex - 1;
					_emplaceDataSet( currentDataset );

					currentDataset					 = BuildingDatasetMetadata( DatasetType::UNKNOWN );
					currentDataset.firstResidueIndex = localResidueIndex;
				}
			}
			else if ( currentDataset.type != DatasetType::STANDARD )
			{
				currentDataset.lastResidueIndex = localResidueIndex - 1;
				_emplaceDataSet( currentDataset );

				currentDataset					 = BuildingDatasetMetadata( DatasetType::STANDARD );
				currentDataset.firstResidueIndex = localResidueIndex;
			}

			previousResidueScaleIndex = residueScaleIndex;
		}

		currentDataset.lastResidueIndex = residueCount - 1;
		_emplaceDataSet( currentDataset );

		_dataset.emplace_back( new Dataset::SequenceDisplayDataset_EndHtmlColorTag( _currentCharPos ) );
	};
	void ChainSequenceBuilder::update()
	{
		clear();
		generate();
	}
	void ChainSequenceBuilder::clear()
	{
		for ( auto it : _dataset )
			delete it;
		_dataset.clear();

		_currentCharPos		= 0;
		_lastDataWasUnknown = false;
	}

	void ChainSequenceBuilder::_emplaceDataSet( const BuildingDatasetMetadata & p_data )
	{
		switch ( p_data.type )
		{
		case DatasetType::STANDARD: _emplaceResidueDataSet( p_data ); break;
		case DatasetType::UNKNOWN: _emplaceUnknownResidueDataSet( p_data ); break;
		case DatasetType::MISSING: _emplaceMissingResidueDataSet( p_data ); break;
		case DatasetType::NOT_SET:
		default: break;
		}

		_lastDataWasUnknown = p_data.type == DatasetType::UNKNOWN;
	}
	void ChainSequenceBuilder::_emplaceResidueDataSet( const BuildingDatasetMetadata & p_data )
	{
		Dataset::SequenceDisplayDataset_Residue * const residueSet = new Dataset::SequenceDisplayDataset_Residue(
			_chain, _currentCharPos, p_data.firstResidueIndex, p_data.lastResidueIndex );
		_dataset.emplace_back( residueSet );

		_currentCharPos += residueSet->getStringSize();
	}
	void ChainSequenceBuilder::_emplaceUnknownResidueDataSet( const BuildingDatasetMetadata & p_data )
	{
		const bool spaceBefore = p_data.firstResidueIndex > 0 && !_lastDataWasUnknown;
		const bool spaceAfter  = p_data.lastResidueIndex < ( _chain.getResidueCount() - 1 );

		Dataset::SequenceDisplayDataset_UnknownResidue * const unknownResidueSet
			= new Dataset::SequenceDisplayDataset_UnknownResidue(
				_chain, _currentCharPos, p_data.firstResidueIndex, p_data.lastResidueIndex, spaceBefore, spaceAfter );
		_dataset.emplace_back( unknownResidueSet );

		_currentCharPos += unknownResidueSet->getStringSize();
	}
	void ChainSequenceBuilder::_emplaceMissingResidueDataSet( const BuildingDatasetMetadata & p_data )
	{
		const uint nbMissingResidue = p_data.lastResidueIndex - p_data.firstResidueIndex + 1;

		Dataset::SequenceDisplayDataset_MissingResidue * const missingResidueSet
			= new Dataset::SequenceDisplayDataset_MissingResidue(
				_currentCharPos, p_data.firstResidueIndex, nbMissingResidue );
		_dataset.emplace_back( missingResidueSet );

		_currentCharPos += missingResidueSet->getStringSize();
	}

	Model::Residue * const ChainSequenceBuilder::_getResidue( const uint p_localResidueIndex ) const
	{
		const uint moleculeResidueIndex = _chain.getIndexFirstResidue() + p_localResidueIndex;
		return _chain.getMoleculePtr()->getResidue( moleculeResidueIndex );
	}

} // namespace VTX::UI::Widget::Sequence
