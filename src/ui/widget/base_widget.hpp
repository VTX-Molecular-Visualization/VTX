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
			template<typename Q, typename T>
			class BaseWidget : public Q, public Event::BaseEventReceiverVTX
			{
			  public:
				BaseWidget( QWidget * p_parent ) : Q( p_parent ), _ui( new T() ) { _ui->setupUi( this ); }
				virtual ~BaseWidget() { delete _ui; }

				inline const T & getUI() const { return _ui; }
				inline T &		 getUI() { return _ui; }

				// Need to override paintEvent to read style on custom widgets
				inline void paintEvent( QPaintEvent * event ) override
				{
					QStyleOption opt;
					opt.init( this );
					QPainter p( this );
					style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );

					Q::paintEvent( event );
				};

			  protected:
				T * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
