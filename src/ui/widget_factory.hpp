#ifndef __VTX_UI_WIDGETFACTORY__
#define __VTX_UI_WIDGETFACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "widget/base_manual_widget.hpp"
#include "widget/base_manual_widget_initializer.hpp"
#include <QMainWindow>
#include <QTreeWidget>
#include <QWidget>
#include <string>
#include <type_traits>

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
			W * getWidget( QWidget * p_parent, const std::string & p_name ) const
			{
				QString qstringName = QString::fromStdString( p_name );

				W * res = new W( p_parent );
				res->_setup( qstringName );

				return res;
			};

			template<typename V, typename M>
			V * getSceneItem( M * const p_model, QTreeWidgetItem * p_parent, const std::string & p_name ) const
			{
				QString qstringName = QString::fromStdString( p_name );

				V * res = new V( p_model, p_parent );
				res->_setup( qstringName );

				return res;
			};

		  private:
			inline WidgetFactory() {};
		};
	} // namespace UI
} // namespace VTX

#endif
