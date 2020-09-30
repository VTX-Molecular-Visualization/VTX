#ifndef __VTX_UI_WIDGET_BASE_WIDGET__
#define __VTX_UI_WIDGET_BASE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			template<typename Q, typename T, typename = std::enable_if<std::is_base_of<QWidget, Q>::value>>
			class BaseWidget : public Q, public T, public Event::BaseEventReceiverVTX
			{
			  public:
				BaseWidget( QWidget * p_parent = 0 ) : Q( p_parent ) { setupUi( this ); }
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
