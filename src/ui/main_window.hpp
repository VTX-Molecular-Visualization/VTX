#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_main_window.h"
#include "vtx_app.hpp"
#include <QMainWindow>
#include <QCloseEvent>

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

		  public slots:
			void on_file_open_triggered();
			void on_file_close_triggered();
			void on_window_togglerender_triggered();
			void on_window_toggleinspector_triggered();
			void on_window_togglescene_triggered();
			void on_window_togglelog_triggered();

		  private:
			Ui_MainWindow * _ui;

			void setupDock();
			void toggleWidget( QWidget * widget );
		};

	} // namespace UI
} // namespace VTX

#endif
