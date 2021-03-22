#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET_RESIDUE_WITH_SAME_ID__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET_RESIDUE_WITH_SAME_ID__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "sequence_dataset.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_ResidueWithSameId : public SequenceDisplayDataset
	{
	  public:
		SequenceDisplayDataset_ResidueWithSameId( const Model::Chain & p_chain,
												  const uint		   p_startIndexChar,
												  const uint		   p_residueIndex,
												  const uint		   p_count );
		void appendToSequence( QString & p_sequenceString ) const override;
		void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, const bool p_startBloc ) const override;

		Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;
		bool				   isResidueInScope( const uint p_residueIndex ) const override;
		uint				   getCharIndexOfResidue( const uint p_residueIndex ) const override;

		Model::Residue * const getFirstResidue() const override
		{
			return _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue()
															  + _startResidueIndex );
		};
		Model::Residue * const getLastResidue() const override
		{
			return _linkedChain.getMoleculePtr()->getResidue( _linkedChain.getIndexFirstResidue() + _endResidueIndex );
		};

		uint getPaintLength( const uint p_charIndex ) const override { return 1; };

	  private:
		const Model::Chain & _linkedChain;
		const uint			 _startResidueIndex;
		const uint			 _endResidueIndex;

		const uint _originalResidueIndex;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
