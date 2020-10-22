#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui_main_window.h"
#include "widget/base_widget.hpp"
#include "widget/console/console_widget.hpp"
#include "widget/inspector/inspector_widget.hpp"
#include "widget/main_menu/main_menu_bar.hpp"
#include "widget/opengl_widget.hpp"
#include "widget/render_widget.hpp"
#include "widget/scene_widget.hpp"
#include "widget/status_bar_widget.hpp"
#include <QCloseEvent>
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
			~MainWindow();

			inline const Widget::OpenGLWidget & getOpenGLWidget() const { return _renderWidget->getOpenGLWidget(); }
			inline Widget::OpenGLWidget &		getOpenGLWidget() { return _renderWidget->getOpenGLWidget(); }

			void receiveEvent( const Event::VTXEvent & p_event ) override;
			void closeEvent( QCloseEvent * ) override;

		  public slots:
			void on_file_open_triggered();
			void on_file_close_triggered();
			void on_window_togglerender_triggered();
			void on_window_toggleinspector_triggered();
			void on_window_togglescene_triggered();
			void on_window_togglelog_triggered();

		  private:
			Widget::MainMenu::MainMenuBar * _mainMenuBar = nullptr;

			Widget::RenderWidget *				 _renderWidget	  = nullptr;
			Widget::SceneWidget *				 _sceneWidget	  = nullptr;
			Widget::Inspector::InspectorWidget * _inspectorWidget = nullptr;
			Widget::Console::ConsoleWidget *	 _consoleWidget	  = nullptr;

			Widget::StatusBarWidget * _statusBarWidget = nullptr;

			void _loadStyleSheet( const char * p_stylesheetPath );
			void _setupSlots();
			void _setupDock();
			void _toggleWidget( QWidget * widget );
		};

	} // namespace UI
} // namespace VTX

#endif
