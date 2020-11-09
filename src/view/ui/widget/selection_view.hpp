#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "view/base_view.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include <QTreeWidgetItem>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionView : public QTreeWidgetItem, public View::BaseView<Model::Selection>, VTX::UI::Widget::BaseManualWidgetInitializer
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					SelectionView( Model::Selection * const p_model, QTreeWidgetItem * const p_parent ) :
						View::BaseView<Model::Selection>( p_model ), QTreeWidgetItem( p_parent ), BaseManualWidgetInitializer()
					{
					}

					void		 localize() override;
					virtual void notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData = 0 ) override;

				  protected:
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

				  private:					
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
