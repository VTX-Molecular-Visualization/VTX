#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_main_window.h"
#include "widget/base_widget.hpp"
#include <QMainWindow>

namespace VTX
{
	namespace UI
	{
		class MainWindow : public Widget::BaseWidget<QMainWindow, Ui_MainWindow>
		{
			Q_OBJECT

		  public:
			MainWindow( QWidget * = 0 );

		  public slots:
			void on_actionTest_triggered();
			void on_window_togglelog_triggered();
		};

	} // namespace UI
} // namespace VTX

#endif
