#ifndef __VTX_VIEW_UI_WIDGET_BASE_WIDGET_VIEW__
#define __VTX_VIEW_UI_WIDGET_BASE_WIDGET_VIEW__

#include "model/base_model.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "ui/widget/view_item_widget.hpp"
#include "ui/widget_factory.hpp"
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
				class BaseWidgetView : public View::BaseView<M>
				{
				  public:
					W * const getWidget() { return _widget; };

				  protected:
					BaseWidgetView( M * const p_model, const std::string & p_widgetName, QWidget * const p_parent = nullptr ) : View::BaseView<M>( p_model )
					{
						_widget = VTX::UI::WidgetFactory::get().instantiateWidget<W>( p_parent, p_widgetName );
						_widget->setModel( p_model );
					};
					~BaseWidgetView() { delete _widget; };

					inline void _refreshView() override { _widget->refresh(); };

				  private:
					W * _widget;
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
