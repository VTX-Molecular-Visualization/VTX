#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_main_window.h"
#include "vtx_app.hpp"
#include <QtWidgets/QMainWindow>

namespace Ui
{
	class MainWindow;
}

namespace VTX
{
	namespace UI
	{
		class MainWindow : public QMainWindow
		{
			Q_OBJECT

		  public:
			MainWindow( QWidget * = 0 );
			~MainWindow();


		  private:
			Ui::MainWindow * _ui;

			
		  public slots:
			void on_actionTest_triggered();
		};

	} // namespace UI
} // namespace VTX

#endif
