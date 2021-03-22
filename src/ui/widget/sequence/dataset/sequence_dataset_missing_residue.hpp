#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET_MISSING_RESIDUE__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET_MISSING_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "sequence_dataset.hpp"
#include "style.hpp"
#include <QString>

namespace VTX::UI::Widget::Sequence::Dataset
{
	class SequenceDisplayDataset_MissingResidue : public SequenceDisplayDataset
	{
		// Use of unbreakable space (U+00A0) instead of normal space because rich text in QLabel
		inline static const QString TOO_LONG_STR = "   ";

	  public:
		SequenceDisplayDataset_MissingResidue( const uint p_startIndexChar,
											   const uint p_startResidueIndexInOriginalChain,
											   const uint p_size ) :
			SequenceDisplayDataset( p_startIndexChar, p_size ),
			_startResidueIndexInOriginalChain( p_startResidueIndexInOriginalChain ),
			_isTooLong( p_size > Style::SEQUENCE_MAX_MISSING_RESIDUE_BEFORE_COLLAPSE )
		{
			_charCount = _isTooLong ? uint( TOO_LONG_STR.size() ) : p_size;
		};

		void appendToSequence( QString & p_sequenceString ) const override;
		void appendToScale( QString & p_scale, uint & p_lastIndexCharWritten, const bool p_startBloc ) const override;

		const bool isFinishingBlock( bool p_startBlock ) const override { return _isTooLong; };

	  private:
		const bool _isTooLong;
		const uint _startResidueIndexInOriginalChain;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
#endif
