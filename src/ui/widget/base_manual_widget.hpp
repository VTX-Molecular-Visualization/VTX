#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget_initializer.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include <QPainter>
#include <QStyleOption>
#include <QWidget>
#include <QtCore>
#include <string>

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

				inline void _setup( const std::string & p_name ) { _setup( QString::fromStdString( p_name ) ); };
				inline void _setup( const QString & p_name )
				{
					_setupUi( p_name );
					_setupSlots();
					localize();
				};

				virtual void _setupUi( const QString & p_name ) override { setObjectName( p_name ); };

				// Need to override paintEvent to read style on custom widgets
				inline void paintEvent( QPaintEvent * event ) override
				{
					QStyleOption opt;
					opt.init( this );
					QPainter p( this );
					style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );

					W::paintEvent( event );
				};
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
