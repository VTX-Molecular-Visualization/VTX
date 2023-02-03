#ifndef __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__

#include "model/chain.hpp"
#include "model/residue.hpp"
#include "sequence_dataset.hpp"

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_UnknownResidue : public SequenceDisplayDataset
	{
	  public:
		SequenceDisplayDataset_UnknownResidue( Model::Residue & p_residue,
											   const bool		p_spaceBefore,
											   const bool		p_spaceAfter,
											   const uint		p_startIndexChar,
											   const uint		p_residueIndex );

		void appendToSequence( QString & p_sequenceString ) const override;
		void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, bool p_startBloc ) const override;
		Model::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;

		virtual bool isResidueInScope( const uint p_residueIndex ) const
		{
			return p_residueIndex == ( _residue->getIndex() - _residue->getChainPtr()->getIndexFirstResidue() );
		};
		virtual uint getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar; };

		Model::Residue * const getFirstResidue() const override { return _residue; };
		Model::Residue * const getLastResidue() const override { return _residue; };

		uint getPaintLength( const uint p_charIndex ) const override { return 3; };
		uint getPaintCharIndex( const uint p_charIndex ) const override
		{
			return _startIndexChar + ( _spaceBefore ? 1 : 0 );
		};
		uint getLastScaleCharIndex() const override;

	  private:
		Model::Residue * const _residue;
		uint				   _residueIndex;
		const bool			   _spaceBefore;
		const bool			   _spaceAfter;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
