#include "qt/application.hpp"
#include "qt/main_window.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>

namespace VTX::UI::QT
{

	void Application::_init( const App::Args & p_args )
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		int zero	  = 0;
		_qApplication = new QApplication( zero, nullptr );
		_qApplication->connect(
			_qApplication, &QApplication::aboutToQuit, [ this ]() { BaseApplication<MainWindow>::stop(); }
		);

		using namespace Resources;
		_qApplication->setWindowIcon( QIcon( ICON_LOGO.toString() ) );
		_qApplication->setApplicationDisplayName( APPLICATION_DISPLAY_NAME.toString() );
		_qApplication->setApplicationName( APPLICATION_NAME.toString() );
		_qApplication->setApplicationVersion( APPLICATION_VERSION.toString() );

		_loadTheme();

		// TODO: move to parent.

		// Internal::initSettings( App::SETTINGS() );
		// App::Core::init( dynamic_cast<App::Mode::Visualization &>( *_currentMode ) );
		//_currentMode->enter();

		// createMenu<Menu::File>();
	}

	void Application::_start()
	{
		// TEST: add meta action.
		addMenuAction( MenuAction { "File", "MENU HOOK TEST" } );
		addToolBarAction( ToolBarAction { "Camera", "TOOL BAR HOOK TEST" } );

		_mainWindow->show();
		_qApplication->exec();
	}

	void Application::_stop() {}

	void Application::_loadTheme()
	{
		using namespace Resources;

		// Load main stylesheet.
		QFile stylesheetFile( FILE_STYLESHEET.toString() );
		stylesheetFile.open( QFile::ReadOnly );
		QString stylesheet = stylesheetFile.readAll();

		// Load os-specific stylesheet.
#if _WIN32
		QFile stylesheetOSFile( FILE_STYLESHEET_WINOWS.toString() );
#elif __linux__
		QFile stylesheetOSFile( FILE_STYLESHEET_LINUX.toString() );
#elif __APPLE__
		QFile stylesheetOSFile( FILE_STYLESHEET_MACOS.toString() );
#else
		QFile stylesheetOSFile();
		assert( true );
#endif

		stylesheetOSFile.open( QFile::ReadOnly );
		stylesheet += '\n' + stylesheetOSFile.readAll();

		// TODO: Load theme file and apply to stylesheet.

		// Set stylesheet to app.
		_qApplication->setStyleSheet( stylesheet );
	}

	void Application::addMenu( QMenu * const p_menu ) { _mainWindow->menuBar()->addMenu( p_menu ); }

	void Application::addToolBar( QToolBar * const p_toolBar ) { _mainWindow->addToolBar( p_toolBar ); }
} // namespace VTX::UI::QT
