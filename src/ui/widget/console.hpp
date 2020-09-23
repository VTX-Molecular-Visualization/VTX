#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "ui_console_widget.h"
#include <QDockWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class Console : public BaseWidget<QDockWidget, Ui_Console>
			{
				Q_OBJECT

			  public:
				Console( QWidget * p_parent );

				virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				std::vector<Event::VTX_EVENT> _getEvents() const override
				{
					return std::vector<Event::VTX_EVENT>( { Event::Global::LOG_CONSOLE } );
				}

			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
