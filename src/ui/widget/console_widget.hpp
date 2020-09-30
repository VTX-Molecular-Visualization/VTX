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
			class ConsoleWidget : public BaseWidget<QDockWidget, Ui_ConsoleWidget>
			{
				Q_OBJECT

			  public:
				ConsoleWidget( QWidget * );

				void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
			  private:
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
