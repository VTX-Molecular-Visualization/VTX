#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/base_event_receiver_vtx.hpp"
#include "ui_console_widget.h"
#include <QDockWidget>


namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class ConsoleWidget : public QDockWidget, public Event::BaseEventReceiverVTX
			{
				Q_OBJECT

			  public:
				  ConsoleWidget( QWidget * p_parent );
				  ~ConsoleWidget();

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				virtual std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::LOG_CONSOLE } );
				}

			  private:
				Ui_ConsoleWidget * _ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
