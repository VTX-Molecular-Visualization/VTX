#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET_TAG__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET_TAG__

#ifdef _MSC_VER
#pragma once
#endif

#include "sequence_dataset.hpp"
#include "util/ui.hpp"

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
					class SequenceDisplayDataset_HtmlTag : public SequenceDisplayDataset
					{
					  public:
						SequenceDisplayDataset_HtmlTag( const uint p_charIndex ) : SequenceDisplayDataset( p_charIndex, p_charIndex ) {};

						int	 getStringSize() const override { return 0; };
						bool isCharIndexInScope( const uint p_charIndex ) override { return false; }
					};

					class SequenceDisplayDataset_HtmlColorTag : public SequenceDisplayDataset_HtmlTag
					{
					  public:
						SequenceDisplayDataset_HtmlColorTag( const uint p_charIndex, const Color::Rgb & p_color );
						const bool isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };
					};

					class SequenceDisplayDataset_EndHtmlColorTag : public SequenceDisplayDataset_HtmlTag
					{
					  public:
						SequenceDisplayDataset_EndHtmlColorTag( const uint p_charIndex ) : SequenceDisplayDataset_HtmlTag( p_charIndex )
						{
							_str = QString();
							Util::UI::appendEndColorHtmlTag( _str );
						};
						const bool isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };
					};

				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
