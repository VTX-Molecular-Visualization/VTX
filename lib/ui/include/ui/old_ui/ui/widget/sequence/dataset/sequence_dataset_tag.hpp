#ifndef __VTX_UI_WIDGET_SEQUENCE_DATASET_TAG__
#define __VTX_UI_WIDGET_SEQUENCE_DATASET_TAG__

#include "sequence_dataset.hpp"
#include "ui/old_ui/util/ui.hpp"

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
						SequenceDisplayDataset_HtmlTag( const uint p_charIndex ) :
							SequenceDisplayDataset( p_charIndex, 0 ) {};

						int	 getStringSize() const override { return 0; };
						bool isCharIndexInScope( const uint p_charIndex ) override { return false; }
					};

					class SequenceDisplayDataset_HtmlColorTag : public SequenceDisplayDataset_HtmlTag
					{
					  public:
						SequenceDisplayDataset_HtmlColorTag( const uint p_charIndex, const Color::Rgba & p_color ) :
							SequenceDisplayDataset_HtmlTag( p_charIndex ), _color( p_color ) {};

						void appendToSequence( QString & p_sequenceString ) const override
						{
							QString tag = QString();
							Util::UI::appendColorHtmlTag( tag, _color );
							p_sequenceString.append( tag );
						}
						const bool isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };

					  protected:
						Color::Rgba _color;
					};

					class SequenceDisplayDataset_EndHtmlColorTag : public SequenceDisplayDataset_HtmlTag
					{
					  public:
						SequenceDisplayDataset_EndHtmlColorTag( const uint p_charIndex ) :
							SequenceDisplayDataset_HtmlTag( p_charIndex ) {};
						void appendToSequence( QString & p_sequenceString ) const override
						{
							QString tag = QString();
							Util::UI::appendEndColorHtmlTag( tag );
							p_sequenceString.append( tag );
						}

						const bool isFinishingBlock( bool p_startBlock ) const override { return p_startBlock; };
					};

				} // namespace Dataset
			}	  // namespace Sequence
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
