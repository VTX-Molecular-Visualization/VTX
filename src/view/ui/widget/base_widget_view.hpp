#ifndef __VTX_VIEW_UI_WIDGET_BASE_WIDGET_VIEW__
#define __VTX_VIEW_UI_WIDGET_BASE_WIDGET_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "ui/widget/view_item_widget.hpp"
#include "ui/widget_factory.hpp"
#include "view/templated_base_view.hpp"
#include "view/ui/linked_widget_interface.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				template<typename M,
						 typename W,
						 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
						 typename = std::enable_if<std::is_base_of<VTX::UI::Widget::ViewItemWidget<M>, W>::value>>
				class BaseWidgetView : public View::TemplatedBaseView<M>, public LinkedWidgetInterface
				{
				  public:
					BaseWidgetView( M * const p_model, const std::string & p_widgetName, QWidget * p_parent = nullptr ) : View::TemplatedBaseView<M>( p_model )
					{
						_widget = VTX::UI::WidgetFactory::get().GetWidget<W>( p_parent, p_widgetName );
						_widget->setModel( p_model );
					};

					void refreshView() override
					{
						TemplatedBaseView<M>::refreshView();
						_widget->refresh();
					};

					W *		  getWidget() { return _widget; };
					QWidget * getLinkedWidget() override { return _widget; }

				  private:
					W * _widget;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif