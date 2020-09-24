#include "main_window.hpp"
#include "vtx_app.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		MainWindow::MainWindow( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			const QSize winsize = QSize( VTX_SETTING().WINDOW_WIDTH_DEFAULT, VTX_SETTING().WINDOW_HEIGHT_DEFAULT );
			resize( winsize );

			addDockWidget( Qt::DockWidgetArea::BottomDockWidgetArea, _ui->console );
		}

		void MainWindow::on_actionTest_triggered() { std::cout << "ACTION TEST" << std::endl; }
		void MainWindow::on_window_togglelog_triggered() { _ui->console->show(); }

	} // namespace UI
} // namespace VTX
