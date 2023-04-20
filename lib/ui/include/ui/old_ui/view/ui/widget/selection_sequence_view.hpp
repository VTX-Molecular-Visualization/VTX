#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__

#include "ui/old_ui/ui/widget/sequence/sequence_widget.hpp"
#include <app/old_app/model/selection.hpp>
#include <app/view/base_view.hpp>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionSequenceView : public View::BaseView<Model::Selection>
				{
					VTX_VIEW

				  protected:
					void _refreshView() override { _linkedSequenceWidget->refreshSelection(); };

				  private:
					const VTX::UI::Widget::Sequence::SequenceWidget * const _linkedSequenceWidget;

					SelectionSequenceView( Model::Selection * const p_model, QWidget * const p_sequenceWidget ) :
						View::BaseView<Model::Selection>( p_model ),
						_linkedSequenceWidget(
							dynamic_cast<VTX::UI::Widget::Sequence::SequenceWidget * const>( p_sequenceWidget ) )
					{
					}
				};
			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
