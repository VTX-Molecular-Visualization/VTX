#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_SEQUENCE_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "ui/widget/sequence/sequence_widget.hpp"
#include "view/base_view.hpp"

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
				  public:
					SelectionSequenceView( Model::Selection * const p_model, const VTX::UI::Widget::Sequence::SequenceWidget * const p_sequenceWidget ) :
						View::BaseView<Model::Selection>( p_model ), _linkedSequenceWidget( p_sequenceWidget ) {};

					virtual void notify( const Event::VTXEvent * const p_event ) override { BaseView::notify( p_event ); };

				  protected:
					void _refreshView() override { _linkedSequenceWidget->refreshSelection(); };

				  private:
					const VTX::UI::Widget::Sequence::SequenceWidget * const _linkedSequenceWidget;
				};
			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
