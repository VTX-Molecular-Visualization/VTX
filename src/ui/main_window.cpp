#include "main_window.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "vtx_app.hpp"
#include "widget_factory.hpp"
#include <QAction>
#include <QFileDialog>
#include <QSettings>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		MainWindow::MainWindow( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			_registerEvent( Event::Global::CHANGE_STATE );

			_mainMenuBar = WidgetFactory::get().instantiateWidget<Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
			this->setMenuBar( _mainMenuBar );

			_renderWidget	 = WidgetFactory::get().instantiateWidget<Widget::Render::RenderWidget>( this, "renderWidget" );
			_sceneWidget	 = WidgetFactory::get().instantiateWidget<Widget::Scene::SceneWidget>( this, "sceneWidget" );
			_inspectorWidget = WidgetFactory::get().instantiateWidget<Widget::Inspector::InspectorWidget>( this, "inspectorWidget" );
			_consoleWidget	 = WidgetFactory::get().instantiateWidget<Widget::Console::ConsoleWidget>( this, "consoleWidget" );
			_sequenceWidget	 = WidgetFactory::get().instantiateWidget<Widget::Sequence::SequenceWidget>( this, "sequenceWidget" );
			_selectionWidget = WidgetFactory::get().instantiateWidget<Widget::Selection::SelectionWidget>( this, "selectionWidget" );

			_statusBarWidget = WidgetFactory::get().instantiateWidget<Widget::StatusBar::StatusBarWidget>( this, "statusBar" );
			_statusBarWidget->setFixedHeight( 25 );
			this->setStatusBar( _statusBarWidget );

			_setupSlots();

			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );
			_setupDock();

			_mainMenuBar->setCurrentTab( 0 );
			_renderWidget->setFocus();

			_loadStyleSheet( VTX_SETTING().STYLESHEET_FILE_DEFAULT );
		}

		void MainWindow::_loadStyleSheet( const char * p_stylesheetPath )
		{
			QFile stylesheetFile( p_stylesheetPath );
			stylesheetFile.open( QFile::ReadOnly );

			QString stylesheet = stylesheetFile.readAll();
			setStyleSheet( stylesheet );
		}

		MainWindow::~MainWindow() {}

		void MainWindow::receiveEvent( const Event::VTXEvent & p_event )
		{
			if ( p_event.name == Event::Global::CHANGE_STATE )
			{
				const Event::VTXEventValue<ID::VTX_ID> & event = dynamic_cast<const Event::VTXEventValue<ID::VTX_ID> &>( p_event );

				ID::VTX_ID state = event.value;
			}
		}

		void MainWindow::_setupSlots() { connect( this->_sequenceWidget, &Widget::Sequence::SequenceWidget::visibilityChanged, this, &MainWindow::_onDockWindowVisibilityChange ); }

		void MainWindow::_setupDock()
		{
			setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks );

			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _sceneWidget, Qt::Orientation::Horizontal );
			splitDockWidget( _sceneWidget, _selectionWidget, Qt::Orientation::Vertical );
			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _sequenceWidget, Qt::Orientation::Horizontal );
			splitDockWidget( _sequenceWidget, _renderWidget, Qt::Orientation::Vertical );
			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea, _inspectorWidget, Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, _consoleWidget, Qt::Orientation::Vertical );

			//_sequenceWidget->hide();
			//_selectionWidget->hide();
		}

		void MainWindow::_onDockWindowVisibilityChange( const bool p_visible ) { VTX_EVENT( new Event::VTXEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE ) ); }

		void MainWindow::closeEvent( QCloseEvent * p_event )
		{
			VTXApp::get().stop();
			p_event->accept();
		}

		void MainWindow::_toggleWidget( QWidget * widget )
		{
			if ( widget->isVisible() )
				widget->hide();
			else
				widget->show();
		}

	} // namespace UI
} // namespace VTX
