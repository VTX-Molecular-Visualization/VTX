#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "ui_console_widget.h"
#include <QDockWidget>
#include <qdockwidget.h>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class Console : public QDockWidget, public Event::BaseEventReceiverVTX
			{
				Q_OBJECT

			  public:
				Console( QWidget * p_parent );
				~Console();

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::LOG_CONSOLE } );
				}

			  private:
				Ui_Console * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
