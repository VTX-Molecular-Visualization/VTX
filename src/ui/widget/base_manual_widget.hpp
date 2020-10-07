#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget_initializer.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
			class BaseManualWidget : public W, public Event::BaseEventReceiverVTX, public BaseManualWidgetInitializer
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  protected:
				BaseManualWidget( QWidget * p_parent = nullptr ) : W( p_parent ), BaseManualWidgetInitializer() {};

				virtual void _setupUi( const QString & p_name ) override { setObjectName( p_name ); };
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
