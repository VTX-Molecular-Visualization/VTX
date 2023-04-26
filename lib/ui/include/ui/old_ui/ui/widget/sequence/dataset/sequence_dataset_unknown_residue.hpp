#ifndef __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__
#define __VTX_UI_WIDGET_SEQUENCE_UNKNOWN_DATASET__

#include "sequence_dataset.hpp"
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_UnknownResidue : public SequenceDisplayDataset
	{
	  private:
		using PairCharLengthCount = std::pair<uint, uint>;

	  public:
		SequenceDisplayDataset_UnknownResidue( const App::Component::Chemistry::Chain & p_chain,
											   const uint			p_startIndexChar,
											   const uint			p_residueFirstIndex,
											   const uint			p_residueLastIndex,
											   const bool			p_spaceBefore,
											   const bool			p_spaceAfter );

		void appendToSequence( QString & p_sequenceString ) const override;
		void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, bool p_startBloc ) const override;
		App::Component::Chemistry::Residue * const getResidueAtCharIndex( const uint p_charIndex ) override;

		virtual bool isResidueInScope( const uint p_residueIndex ) const
		{
			return _residueFirstIndex <= p_residueIndex && p_residueIndex <= _residueLastIndex;
		};
		virtual uint getCharIndexOfResidue( const uint p_residueIndex ) const { return _startIndexChar; };

		App::Component::Chemistry::Residue * const getFirstResidue() const override
		{
			return _chain->getMoleculePtr()->getResidue( _chain->getIndexFirstResidue() + _residueFirstIndex );
		};
		App::Component::Chemistry::Residue * const getLastResidue() const override
		{
			return _chain->getMoleculePtr()->getResidue( _chain->getIndexFirstResidue() + _residueLastIndex );
		};

		uint getPaintLength( const uint p_charIndex ) const override;
		uint getPaintCharIndex( const uint p_residueIndex ) const override;
		uint getLastScaleCharIndex() const override;

	  private:
		void _buildInnerData();

		const App::Component::Chemistry::Chain * const _chain;
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
