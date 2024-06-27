#include "qt/application.hpp"
#include "qt/main_window.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QSplashScreen>

namespace VTX::UI::QT
{

	int zero = 0;
	Application::Application() : UI::BaseApplication<MainWindow>() { VTX_DEBUG( "Application()" ); }

	void Application::_init( const App::Args & p_args )
	{
		using namespace Resources;

		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		int zero	  = 0;
		_qApplication = new QApplication( zero, nullptr );

		_qApplication->setWindowIcon( QIcon( SPRITE_LOGO.toString() ) );
		_qApplication->setApplicationDisplayName( APPLICATION_DISPLAY_NAME.toString() );
		_qApplication->setApplicationName( APPLICATION_NAME.toString() );
		_qApplication->setApplicationVersion( APPLICATION_VERSION.toString() );
		_qApplication->setOrganizationName( ORGANIZATION_NAME.toString() );
		_qApplication->setOrganizationDomain( ORGANIZATION_DOMAIN.toString() );

		_qSplashScreen = new QSplashScreen( QPixmap( SPRITE_SPLASH.toString() ) );
		_qSplashScreen->show();
		_qSplashScreen->showMessage( "Loading..." );

		_loadTheme();

		// TODO: move to parent.

		// Internal::initSettings( App::SETTINGS() );
		// App::Core::init( dynamic_cast<App::Mode::Visualization &>( *_currentMode ) );
		//_currentMode->enter();

		// createMenu<Menu::File>();

		_qApplication->connect(
			_qApplication, &QApplication::aboutToQuit, [ this ]() { BaseApplication<MainWindow>::stop(); }
		);
	}

	void Application::_start()
	{
		// TEST: add meta action.
		addMenuAction( MenuAction { "File", "MENU HOOK TEST" } );
		addToolBarAction( ToolBarAction { "Camera", "TOOL BAR HOOK TEST" } );

		// Restore settings.
		QSettings settings;
		_mainWindow->restoreGeometry( settings.value( "geometry" ).toByteArray() );
		_mainWindow->restoreState( settings.value( "windowState" ).toByteArray() );

		// Show app.
		_qSplashScreen->finish( _mainWindow.get() );
		_mainWindow->show();
		_qApplication->exec();
	}

	void Application::_stop()
	{
		// Save settings.
		QSettings settings;
		settings.setValue( "geometry", _mainWindow->saveGeometry() );
		settings.setValue( "windowState", _mainWindow->saveState() );
		settings.sync();

		if ( settings.status() != QSettings::NoError )
		{
			VTX_ERROR( "Failed to save Qt settings." );
		}
	}

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

	void Application::addDockWidget( const Qt::DockWidgetArea p_area, QDockWidget * const p_dockWidget )
	{
		_mainWindow->addDockWidget( p_area, p_dockWidget );
	}
} // namespace VTX::UI::QT
