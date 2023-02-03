#include "sequence_dataset.hpp"
#include <cmath>

namespace VTX::UI::Widget::Sequence::Dataset
{
	uint SequenceDisplayDataset::_drawInScale( QString &		   p_scale,
											   const std::string & p_label,
											   const uint		   p_charIndex,
											   const bool		   p_center ) const
	{
		const uint indexTxtSize = (uint)p_label.size();
		const uint indexOffset	= p_center ? ( indexTxtSize - 1 ) / 2 : 0;

		for ( uint j = 0; j < indexTxtSize; j++ )
		{
			const uint index = p_charIndex + j - indexOffset;
			p_scale[ index ] = p_label[ j ];
		}

		return p_charIndex - indexOffset + indexTxtSize - 1;
	}

	uint SequenceDisplayDataset::_getStepToNextValidIndex( const int p_originalIndex ) const
	{
		const int moduloStep = p_originalIndex % Style::SEQUENCE_CHAIN_SCALE_STEP;

		uint step;
		if ( moduloStep < 0 )
			step = std::abs( moduloStep );
		else if ( moduloStep > 0 )
			step = Style::SEQUENCE_CHAIN_SCALE_STEP - moduloStep;
		else
			step = 0;

		return step;
	};
} // namespace VTX::UI::Widget::Sequence::Dataset
