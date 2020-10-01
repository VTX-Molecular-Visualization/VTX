#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_main_window.h"
#include "widget/base_widget.hpp"
#include "widget/console_widget.hpp"
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

			void receiveEvent( const Event::VTXEvent & p_event ) override;

		  public slots:
			void on_file_open_triggered();
			void on_file_close_triggered();
			void on_window_togglerender_triggered();
			void on_window_toggleinspector_triggered();
			void on_window_togglescene_triggered();
			void on_window_togglelog_triggered();

		  private:
			Widget::ConsoleWidget * _consoleWidget;

			void _loadStyleSheet( const char * p_stylesheetPath );
			void _setupSlots();
			void _setupDock();
			void _toggleWidget( QWidget * widget );
		};

	} // namespace UI
} // namespace VTX

#endif
