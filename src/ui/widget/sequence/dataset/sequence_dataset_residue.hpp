#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET_RESIDUE__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET_RESIDUE__

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "sequence_dataset.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_Residue : public SequenceDisplayDataset
	{
	  public:
		SequenceDisplayDataset_Residue( const Model::Chain & p_chain,
										const uint			 p_startIndexChar,
										const uint			 p_startResidueIndex,
										const uint			 p_endResidueIndex );
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
		uint				 _startResidueIndex;
		uint				 _endResidueIndex;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
