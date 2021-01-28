#ifndef __VTX_UI_MAIN_WINDOW__
#define __VTX_UI_MAIN_WINDOW__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui_main_window.h"
#include "widget/base_widget.hpp"
#include "widget/console/console_widget.hpp"
#include "widget/inspector/inspector_widget.hpp"
#include "widget/main_menu/main_menu_bar.hpp"
#include "widget/render/opengl_widget.hpp"
#include "widget/render/render_widget.hpp"
#include "widget/scene/scene_widget.hpp"
#include "widget/selection/selection_widget.hpp"
#include "widget/sequence/sequence_widget.hpp"
#include "widget/settings/setting_widget.hpp"
#include "widget/status_bar/status_bar_widget.hpp"
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

			inline const Widget::Render::OpenGLWidget & getOpenGLWidget() const
			{
				return _renderWidget->getOpenGLWidget();
			}
			inline Widget::Render::OpenGLWidget & getOpenGLWidget() { return _renderWidget->getOpenGLWidget(); }

			void receiveEvent( const Event::VTXEvent & p_event ) override;
			void closeEvent( QCloseEvent * ) override;

			bool getWidgetVisibility( const ID::VTX_ID & p_winId ) const;
			void toggleSequenceWindow() const;
			void openSettingWindow() const;
			void openSettingWindow( const Widget::Settings::SETTING_MENU & p_menuIndex ) const;

		  private:
			Widget::MainMenu::MainMenuBar * _mainMenuBar = nullptr;

			Widget::Render::RenderWidget *		 _renderWidget	  = nullptr;
			Widget::Scene::SceneWidget *		 _sceneWidget	  = nullptr;
			Widget::Inspector::InspectorWidget * _inspectorWidget = nullptr;
			Widget::Console::ConsoleWidget *	 _consoleWidget	  = nullptr;
			Widget::Sequence::SequenceWidget *	 _sequenceWidget  = nullptr;
			Widget::Selection::SelectionWidget * _selectionWidget = nullptr;
			Widget::Settings::SettingWidget *	 _settingWidget	  = nullptr;

			Widget::StatusBar::StatusBarWidget * _statusBarWidget = nullptr;

			// Actions.
			void _onDockWindowVisibilityChange( bool p_visible );

			// Functions.
			inline const QWidget & getWidget( const ID::VTX_ID & p_winId ) const
			{
				const QWidget * widget = nullptr;

				if ( p_winId == ID::UI::Window::RENDER )
					widget = _renderWidget;
				else if ( p_winId == ID::UI::Window::SCENE )
					widget = _sceneWidget;
				else if ( p_winId == ID::UI::Window::INSPECTOR )
					widget = _inspectorWidget;
				else if ( p_winId == ID::UI::Window::CONSOLE )
					widget = _consoleWidget;
				else if ( p_winId == ID::UI::Window::SEQUENCE )
					widget = _sequenceWidget;
				else if ( p_winId == ID::UI::Window::SELECTION )
					widget = _selectionWidget;
				else if ( p_winId == ID::UI::Window::SETTINGS )
					widget = _settingWidget;

				return *widget;
			}

			void _loadStyleSheet( const char * p_stylesheetPath );
			void _setupSlots();
			void _setupDock();
			void _toggleWidget( QWidget * widget );
		};

	} // namespace UI
} // namespace VTX

#endif
