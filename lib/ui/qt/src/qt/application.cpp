#include "qt/application.hpp"
#include "qt/main_window.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QSplashScreen>
#include <app/filesystem.hpp>
#include <app/infos.hpp>
#include <util/enum.hpp>

namespace VTX::UI::QT
{
	// Need to be called before QApplication constructor.
	void Application::configure()
	{
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
	}

	// Create QApplication with zero argc and nullptr argv.
	int zero = 0;
	Application::Application() :
		UI::BaseApplication<MainWindow>(), QApplication( zero, nullptr ),
		_settings( QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat )
	{
		using namespace Resources;
		using namespace VTX::App::Info;

		_qSplashScreen = new QSplashScreen( QPixmap( SPRITE_SPLASH ) );
		_qSplashScreen->show();
		_qSplashScreen->showMessage( "Loading..." );

		setWindowIcon( QIcon( SPRITE_LOGO ) );
		setApplicationDisplayName( QString::fromStdString( APPLICATION_DISPLAY_NAME ) );
		setApplicationName( QString::fromStdString( APPLICATION_NAME ) );
		setApplicationVersion( QString::fromStdString( APPLICATION_VERSION ) );
		setOrganizationName( QString::fromStdString( ORGANIZATION_NAME ) );
		setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN ) );

		_loadTheme();
	}

	Application::~Application()
	{
		try
		{
			_saveSettings();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to save settings: {}", e.what() );
		}
	}

	void Application::_start()
	{
		try
		{
			// Restore settings after main window is built.
			_restoreSettings();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to restore settings: {}", e.what() );
		}

		// Show.
		_qSplashScreen->finish( _mainWindow.get() );
		_mainWindow->show();

		onStop += [ this ]
		{
			_timer.stop();
			_elapsedTimer.invalidate();

			_mainWindow->close();
			QApplication::quit();
		};

		// Run main loop.
		connect(
			this, &QCoreApplication::aboutToQuit, [] { App::VTX_ACTION().execute<App::Action::Application::Quit>(); }
		);
		connect( &_timer, &QTimer::timeout, [ this ] { update( _elapsedTimer.elapsed() ); } );
		_timer.start( 0 );
		_elapsedTimer.start();

		// Then block.
		exec();
	}

	void Application::_loadTheme()
	{
		using namespace Resources;

		// Load main stylesheet.
		QFile stylesheetFile( FILE_STYLESHEET );
		stylesheetFile.open( QFile::ReadOnly );
		QString stylesheet = stylesheetFile.readAll();

		// Load os-specific stylesheet.
#if _WIN32
		QFile stylesheetOSFile( FILE_STYLESHEET_WINOWS );
#elif __linux__
		QFile stylesheetOSFile( FILE_STYLESHEET_LINUX );
#elif __APPLE__
		QFile stylesheetOSFile( FILE_STYLESHEET_MACOS );
#else
		QFile stylesheetOSFile();
		assert( true );
#endif

		stylesheetOSFile.open( QFile::ReadOnly );
		stylesheet += '\n' + stylesheetOSFile.readAll();

		// TODO: Load theme file and apply to stylesheet.

		// Set stylesheet to app.
		//_qApplication->setStyleSheet( stylesheet );
		setStyle( "fusion" );

		QPalette p = palette();

		/*
		palette.setBrush( QPalette::Window, Qt::Dense3Pattern );
		palette.setBrush( QPalette::WindowText, Qt::white );
		palette.setBrush( QPalette::Base, QColor( 25, 25, 25 ) );
		palette.setBrush( QPalette::AlternateBase, QColor( 53, 53, 53 ) );
		palette.setBrush( QPalette::ToolTipBase, Qt::white );
		palette.setBrush( QPalette::ToolTipText, Qt::white );
		palette.setBrush( QPalette::Text, Qt::white );
		palette.setBrush( QPalette::Button, QColor( 53, 53, 53 ) );
		palette.setBrush( QPalette::ButtonText, Qt::white );
		palette.setBrush( QPalette::BrightText, Qt::red );
		 palette.setColor(QPalette::Link, QColor(0, 0, 255));
		palette.setBrush( QPalette::Highlight, QColor( 142, 45, 197 ).lighter() );
		palette.setBrush( QPalette::HighlightedText, Qt::black );
		*/

		setPalette( p );
	}

	void Application::_saveSettings()
	{
		VTX_INFO( "Saving settings: {}", _settings.fileName().toStdString() );
		_settings.setValue( "geometry", _mainWindow->saveGeometry() );
		_settings.setValue( "windowState", _mainWindow->saveState() );
		_settings.sync();

		if ( _settings.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( _settings.status() ) ) );
		}
	}

	void Application::_restoreSettings()
	{
		if ( _settings.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( _settings.status() ) ) );
		}

		VTX_INFO( "Restoring settings: {}", _settings.fileName().toStdString() );
		_mainWindow->restoreGeometry( _settings.value( "geometry" ).toByteArray() );
		_mainWindow->restoreState( _settings.value( "windowState" ).toByteArray() );
	}

} // namespace VTX::UI::QT
