#ifndef __VTX_UI_WIDGETFACTORY__
#define __VTX_UI_WIDGETFACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "base_manual_widget_initializer.hpp"
#include <QMainWindow>
#include <QWidget>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class WidgetFactory
			{
			  public:
				inline static WidgetFactory & get()
				{
					static WidgetFactory instance;
					return instance;
				};

			  public:
				template<typename W, typename = std::enable_if<std::is_base_of<W, Widget::BaseManualWidgetInitializer>::value>>
				W * GetWidget( QWidget * p_parent = nullptr ) const
				{
					W * res = new W( p_parent );

					Widget::BaseManualWidgetInitializer * widget = static_cast<Widget::BaseManualWidgetInitializer *>( res );

					widget->setupUi();
					widget->setupSlots();
					widget->localize();

					return res;
				};

			  private:
				inline WidgetFactory() {};
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
