#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__

#include "ui/old_ui/ui/widget/sequence/sequence_widget.hpp"
#include <app/old/application/selection/selection.hpp>
#include <app/old/core/view/base_view.hpp>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionSequenceView : public App::Old::Core::View::BaseView<App::Old::Application::Selection::SelectionModel>
				{
					VTX_VIEW

				  protected:
					void _refreshView() override { _linkedSequenceWidget->refreshSelection(); };

				  private:
					const VTX::UI::Widget::Sequence::SequenceWidget * const _linkedSequenceWidget;

					SelectionSequenceView( App::Old::Application::Selection::SelectionModel * const p_model, QWidget * const p_sequenceWidget ) :
						App::Old::Core::View::BaseView<App::Old::Application::Selection::SelectionModel>( p_model ),
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
