#include "sequence_dataset_tag.hpp"

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
					SequenceDisplayDataset_HtmlColorTag::SequenceDisplayDataset_HtmlColorTag( const uint p_charIndex, const Color::Rgb & p_color ) :
						SequenceDisplayDataset_HtmlTag( p_charIndex )
					{
						_str = QString();
						Util::UI::appendColorHtmlTag( _str, p_color );
					};

				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
