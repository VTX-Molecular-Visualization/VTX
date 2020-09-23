#include "main_window.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		MainWindow::MainWindow( QWidget * p_parent ) : QMainWindow( p_parent ), _ui( new Ui_MainWindow() )
		{
			_ui->setupUi( this );

			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );

			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, _ui->console );
		}

		MainWindow::~MainWindow() { delete _ui; }

		void MainWindow::on_actionTest_triggered() { std::cout << "ACTION TEST" << std::endl; }
		void MainWindow::on_window_togglelog_triggered()
		{
			_ui->console->show();
		}

	} // namespace UI
} // namespace VTX
