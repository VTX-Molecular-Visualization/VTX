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

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				namespace Dataset
				{
					class SequenceDisplayDataset_MissingResidue : public SequenceDisplayDataset
					{
					  public:
						SequenceDisplayDataset_MissingResidue( const uint p_startIndexChar, const uint p_startResidueIndexInOriginalChain, const uint p_size ) :
							SequenceDisplayDataset( p_startIndexChar, p_startIndexChar + p_size - 1 ), _startResidueIndexInOriginalChain( p_startResidueIndexInOriginalChain ),
							_size( p_size ), _isTooLong( p_size > Style::SEQUENCE_MAX_MISSING_RESIDUE_BEFORE_COLLAPSE )
						{
							if ( _isTooLong )
							{
								// Use of unbreakable space (U+00A0) instead of normal space because rich text in QLabel collapse consecutive space and break scale alignment
								_str		  = QString( " " );
								_endIndexChar = _startIndexChar;
								_charCount	  = 1;
							}
							else
							{
								_str = QString( p_size, '-' );
								//_charCount = _size;
							}
						};
						const void appendToScale( QString & p_scale, const bool p_startBloc ) const override;

						const bool isFinishingBlock( bool p_startBlock ) const override { return _isTooLong; };

					  private:
						const bool _isTooLong;
						const uint _startResidueIndexInOriginalChain;
						const uint _size;
					};
				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
