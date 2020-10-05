#ifndef __VTX_UI_WIDGETFACTORY__
#define __VTX_UI_WIDGETFACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "widget/base_manual_widget.hpp"
#include "widget/base_manual_widget_initializer.hpp"
#include <QMainWindow>
#include <QWidget>

namespace VTX
{
	namespace UI
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
			W * GetWidget( QWidget * p_parent, const QString & p_name ) const
			{
				W * res = new W( p_parent );

				res->_setupUi( p_name );
				res->_setupSlots();
				res->localize();

				return res;
			};

		  private:
			inline WidgetFactory() {};
		};
	} // namespace UI
} // namespace VTX

#endif
