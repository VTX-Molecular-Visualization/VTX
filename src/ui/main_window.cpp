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

			_consoleWidget = WidgetFactory::get().GetWidget<Widget::ConsoleWidget>();

			_setupSlots();

			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );
			_setupDock();

			_loadStyleSheet( VTX_SETTING().STYLESHEET_FILE_DEFAULT );
		}

		void MainWindow::_loadStyleSheet( const char * p_stylesheetPath )
		{
			QFile stylesheetFile( p_stylesheetPath );
			stylesheetFile.open( QFile::ReadOnly );

			QString stylesheet = stylesheetFile.readAll();
			setStyleSheet( stylesheet );
		}

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
			this->setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks | DockOption::AnimatedDocks );

			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, this->inspectorWidget, Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, this->renderWidget, Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, this->sceneWidget, Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, this->_consoleWidget, Qt::Orientation::Vertical );

			resizeDocks( { this->renderWidget, this->_consoleWidget },
						 { VTX_SETTING().RENDER_WIDGET_HEIGHT_DEFAULT, VTX_SETTING().CONSOLE_WIDGET_HEIGHT_DEFAULT },
						 Qt::Orientation::Vertical );
			resizeDocks( { this->sceneWidget, this->inspectorWidget },
						 { VTX_SETTING().SCENE_WIDGET_WIDTH_DEFAULT, VTX_SETTING().INSPECTOR_WIDGET_WIDTH_DEFAULT },
						 Qt::Orientation::Horizontal );
		}

		void MainWindow::on_file_open_triggered()
		{
			// TODO : Filter file type
			const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", "*" );
			Path *		  path	   = new Path( filename.toStdString() );

			VTX_ACTION( new Action::Main::Open( path ), true );
		}
		void MainWindow::on_file_close_triggered() { close(); }

		void MainWindow::on_window_togglerender_triggered() { _toggleWidget( this->renderWidget ); }
		void MainWindow::on_window_toggleinspector_triggered() { _toggleWidget( this->inspectorWidget ); }
		void MainWindow::on_window_togglescene_triggered() { _toggleWidget( this->sceneWidget ); }
		void MainWindow::on_window_togglelog_triggered() { _toggleWidget( this->_consoleWidget ); }

		void MainWindow::_toggleWidget( QWidget * widget )
		{
			if ( widget->isVisible() )
				widget->hide();
			else
				widget->show();
		}

	} // namespace UI
} // namespace VTX
