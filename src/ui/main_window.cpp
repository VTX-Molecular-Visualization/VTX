#include "main_window.hpp"
#include "action/main.hpp"
#include "vtx_app.hpp"
#include <iostream>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>


namespace VTX
{
	namespace UI
	{
		MainWindow::MainWindow( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );

			setupDock();
		}

		void MainWindow::toggleWidget( QWidget * widget )
		{
			if ( widget->isVisible() )
				widget->hide();
			else
				widget->show();
		}

		void MainWindow::on_window_togglelog_triggered() { _ui->console->show(); }

		void MainWindow::setupDock()
		{
			this->setDockOptions( DockOption::VerticalTabs | DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks
								  | DockOption::AnimatedDocks );

			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea,		_ui->inspector,		Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea,		_ui->render,		Qt::Orientation::Horizontal);
			addDockWidget( Qt::DockWidgetArea::TopDockWidgetArea,		_ui->scene,			Qt::Orientation::Horizontal );
			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea,	_ui->console,		Qt::Orientation::Vertical );

			resizeDocks({ _ui->render, _ui->console },		{ VTX_SETTING().RENDER_WIDGET_HEIGHT_DEFAULT,	VTX_SETTING().CONSOLE_WIDGET_HEIGHT_DEFAULT },		Qt::Orientation::Vertical);
			resizeDocks({ _ui->scene, _ui->inspector },		{ VTX_SETTING().SCENE_WIDGET_WIDTH_DEFAULT,		VTX_SETTING().INSPECTOR_WIDGET_WIDTH_DEFAULT},		Qt::Orientation::Horizontal);
		}

		
		void MainWindow::on_file_open_triggered()
		{
			//TODO : Filter file type
			const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", "*" );
			Path*		  path = new Path( filename.toStdString() );

			VTX_ACTION( new Action::Main::Open(path), true );
		}
		void MainWindow::on_file_close_triggered()
		{
			close();
		}

		void MainWindow::on_window_togglerender_triggered() { toggleWidget( _ui->render ); }
		void MainWindow::on_window_toggleinspector_triggered() { toggleWidget( _ui->inspector ); }
		void MainWindow::on_window_togglescene_triggered() { toggleWidget( _ui->scene ); }
		void MainWindow::on_window_togglelog_triggered() { toggleWidget( _ui->console ); }

	} // namespace UI
} // namespace VTX
