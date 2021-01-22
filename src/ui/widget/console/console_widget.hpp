#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QDockWidget>
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Console
			{
				class ConsoleWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_WIDGET

				  public:
					~ConsoleWidget();
					void receiveEvent( const Event::VTXEvent & p_event ) override;

				  protected:
					ConsoleWidget( QWidget * );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;
					void localize() override;

				  private:
					CustomWidget::DockWindowMainWidget<QListWidget> * _listWidget = nullptr;
				};
			} // namespace Console
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
