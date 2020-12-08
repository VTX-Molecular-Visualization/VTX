#ifndef __VTX_UI_WIDGETFACTORY__
#define __VTX_UI_WIDGETFACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "mvc/mvc_manager.hpp"
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
			}

		  public:
			template<typename W, typename = std::enable_if<std::is_base_of<W, Widget::BaseManualWidgetInitializer>::value>>
			W * const instanciateWidget( QWidget * const p_parent, const std::string & p_name ) const
			{
				QString qstringName = QString::fromStdString( p_name );

				W * const res = new W( p_parent );

				res->blockSignals( true );
				res->setUpdatesEnabled( false );
				res->_setup( qstringName );
				res->setUpdatesEnabled( true );
				res->blockSignals( false );

				return res;
			}

			template<typename V, typename M, typename W>
			V * const instanciateViewWidget( M * const p_model, const ID::VTX_ID & p_id, QWidget * const p_parent, const std::string & p_name ) const
			{
				QString qstringName = QString::fromStdString( p_name );

				V * const res = MVC::MvcManager::get().instanciateViewWidget<V>( p_model, p_id, p_parent );

				res->blockSignals( true );
				res->setUpdatesEnabled( false );
				res->_setup( qstringName );
				res->setUpdatesEnabled( true );
				res->blockSignals( false );

				return res;
			}

		  private:
			WidgetFactory() = default;
		};
	} // namespace UI
} // namespace VTX

#endif
