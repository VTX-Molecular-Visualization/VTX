#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QDockWidget>
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class ConsoleWidget : public BaseManualWidget<QDockWidget>
			{
			  public:
				ConsoleWidget( QWidget * );

				void receiveEvent( const Event::VTXEvent & p_event ) override;

			  protected:
				void setupUi() override;
				void setupSlots() override;
				void localize() override;

			  private:
				QListWidget * listWidget = 0;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
