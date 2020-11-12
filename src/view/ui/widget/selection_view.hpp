#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/selection.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "view/base_view.hpp"
#include <QTreeWidgetItem>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionView : public View::BaseView<Model::Selection>, VTX::UI::Widget::BaseManualWidgetInitializer, public QTreeWidgetItem
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void		 localize() override;
					virtual void notify( const Event::Model & p_event, const Event::VTXEventModelData * const p_eventData = 0 ) override;

				  protected:
					SelectionView( Model::Selection * const p_model, QTreeWidgetItem * const p_parent ) :
						View::BaseView<Model::Selection>( p_model ), BaseManualWidgetInitializer(), QTreeWidgetItem( p_parent )
					{
					}
					void _setupUi( const QString & ) override;
					void _setupSlots() override;
					void refreshView() override;
					void clear();

				  private:
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
