#include "main_window.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		MainWindow::MainWindow( QWidget * p_parent ) : QMainWindow( p_parent ), _ui( new Ui::MainWindow() )
		{
			_ui->setupUi( this );

			// connect( _ui->actionTest, &QAction::triggered, this, &MainWindow::actionTest );
		}

		MainWindow::~MainWindow() { delete _ui; }

		void MainWindow::actionTest() { std::cout << "ACTION TEST" << std::endl; }

	} // namespace UI
} // namespace VTX
