#include "main_window.hpp"
#include "action/main.hpp"
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

			_mainMenuBar = WidgetFactory::get().GetWidget<Widget::MainMenu::MainMenuBar>( this, "mainMenuBar" );
			this->setMenuBar( _mainMenuBar );

			_renderWidget	 = WidgetFactory::get().GetWidget<Widget::Render::RenderWidget>( this, "renderWidget" );
			_sceneWidget	 = WidgetFactory::get().GetWidget<Widget::Scene::SceneWidget>( this, "sceneWidget" );
			_inspectorWidget = WidgetFactory::get().GetWidget<Widget::Inspector::InspectorWidget>( this, "inspectorWidget" );
			_consoleWidget	 = WidgetFactory::get().GetWidget<Widget::Console::ConsoleWidget>( this, "consoleWidget" );
			_sequenceWidget	 = WidgetFactory::get().GetWidget<Widget::Sequence::SequenceWidget>( this, "sequenceWidget" );
			_selectionWidget = WidgetFactory::get().GetWidget<Widget::Selection::SelectionWidget>( this, "selectionWidget" );

			_statusBarWidget = WidgetFactory::get().GetWidget<Widget::StatusBar::StatusBarWidget>( this, "statusBar" );
			_statusBarWidget->setFixedHeight( 25 );
			this->setStatusBar( _statusBarWidget );

			_setupSlots();

			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );
			_setupDock();

			_mainMenuBar->setCurrentTab( 0 );

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

		void MainWindow::_setupSlots()
		{
			connect( this->file_open, &QAction::triggered, this, &MainWindow::on_file_open_triggered );
			connect( this->file_close, &QAction::triggered, this, &MainWindow::on_file_close_triggered );
			connect( this->window_togglerender, &QAction::triggered, this, &MainWindow::on_window_togglerender_triggered );
			connect( this->window_toggleinspector, &QAction::triggered, this, &MainWindow::on_window_toggleinspector_triggered );
			connect( this->window_togglescene, &QAction::triggered, this, &MainWindow::on_window_togglescene_triggered );
			connect( this->window_togglelog, &QAction::triggered, this, &MainWindow::on_window_togglelog_triggered );
		}

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

			resizeDocks( { _renderWidget, _consoleWidget }, { 500, 1 }, Qt::Orientation::Vertical );
			resizeDocks( { _sceneWidget, _renderWidget, _inspectorWidget }, { 1, 5, 1 }, Qt::Orientation::Horizontal );
		}

		void MainWindow::closeEvent( QCloseEvent * p_event )
		{
			VTXApp::get().stop();
			p_event->accept();
		}

		void MainWindow::on_file_open_triggered()
		{
			// TODO : Filter file type
			const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", "*" );
			Path *		  path	   = new Path( filename.toStdString() );

			VTX_ACTION( new Action::Main::Open( path ), true );
		}
		void MainWindow::on_file_close_triggered() { close(); }

		void MainWindow::on_window_togglerender_triggered() { _toggleWidget( _renderWidget ); }
		void MainWindow::on_window_toggleinspector_triggered() { _toggleWidget( _inspectorWidget ); }
		void MainWindow::on_window_togglescene_triggered() { _toggleWidget( _sceneWidget ); }
		void MainWindow::on_window_togglelog_triggered() { _toggleWidget( _consoleWidget ); }

		void MainWindow::_toggleWidget( QWidget * widget )
		{
			if ( widget->isVisible() )
				widget->hide();
			else
				widget->show();
		}

	} // namespace UI
} // namespace VTX
