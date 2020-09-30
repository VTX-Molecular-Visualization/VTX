#ifndef __VTX_UI_WIDGET_BASE_WIDGET__
#define __VTX_UI_WIDGET_BASE_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include <QPainter>
#include <QStyleOption>
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

				// Need to override paintEvent to read style on custom widgets
				inline void paintEvent( QPaintEvent * event ) override
				{
					QStyleOption opt;
					opt.init( this );
					QPainter p( this );
					style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );

					Q::paintEvent( event );
				};
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
