#ifndef __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "sequence_dataset.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_UnknownResidue : public SequenceDisplayDataset
	{
	  private:
		using PairCharLengthCount = std::pair<uint, uint>;

	  public:
		SequenceDisplayDataset_UnknownResidue( const Model::Chain & p_chain,
											   const uint			p_startIndexChar,
											   const uint			p_residueFirstIndex,
											   const uint			p_residueLastIndex,
											   const bool			p_spaceBefore,
											   const bool			p_spaceAfter );

		void appendToSequence( QString & p_sequenceString ) const override;
		void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, bool p_startBloc ) const override;
		Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;

		virtual bool isResidueInScope( const uint p_residueIndex ) const
		{
			return _residueFirstIndex <= p_residueIndex && p_residueIndex <= _residueLastIndex;
		};
		virtual uint getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar; };

		Model::Residue * const getFirstResidue() const override
		{
			return _chain->getMoleculePtr()->getResidue( _residueFirstIndex );
		};
		Model::Residue * const getLastResidue() const override
		{
			return _chain->getMoleculePtr()->getResidue( _residueLastIndex );
		};

		uint getPaintLength( const uint p_charIndex ) const override;
		uint getPaintCharIndex( const uint p_residueIndex ) const override;
		uint getLastScaleCharIndex() const override;

	  private:
		void _buildInnerData();

		const Model::Chain * const _chain;
		uint					   _residueFirstIndex;
		uint					   _residueLastIndex;
		const bool				   _spaceBefore;
		const bool				   _spaceAfter;

		std::vector<PairCharLengthCount> _charLengthCount = std::vector<PairCharLengthCount>();
		int								 getPairIndexFromCharIndex( const uint p_charIndex ) const;
		int								 getPairIndexFromResidueIndex( const uint p_residueIndex ) const;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
