#include "qt/application.hpp"
#include "qt/main_window.hpp"
#include "qt/menu/file.hpp"
#include "qt/resources.hpp"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QSplashScreen>
// #include <app/action/application.hpp>
// #include <app/application/system/action_manager.hpp>
#include <app/filesystem.hpp>
#include <app/infos.hpp>
#include <util/enum.hpp>

namespace VTX::UI::QT
{

	Application::Application() : UI::BaseApplication<MainWindow>() { VTX_DEBUG( "Application()" ); }

	Application::~Application()
	{
		VTX_DEBUG( "~Application()" );

		try
		{
			_saveSettings();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to save settings: {}", e.what() );
		}
	}

	void Application::_init( const App::Args & p_args )
	{
		using namespace Resources;
		using namespace VTX::App::Info;

		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		int zero	  = 0;
		_qApplication = new QApplication( zero, nullptr );

		_qApplication->setWindowIcon( QIcon( SPRITE_LOGO ) );
		_qApplication->setApplicationDisplayName( QString::fromStdString( APPLICATION_DISPLAY_NAME ) );
		_qApplication->setApplicationName( QString::fromStdString( APPLICATION_NAME ) );
		_qApplication->setApplicationVersion( QString::fromStdString( APPLICATION_VERSION ) );
		_qApplication->setOrganizationName( QString::fromStdString( ORGANIZATION_NAME ) );
		_qApplication->setOrganizationDomain( QString::fromStdString( ORGANIZATION_DOMAIN ) );

		QSettings::setDefaultFormat( QSettings::IniFormat );

		_qSplashScreen = new QSplashScreen( QPixmap( SPRITE_SPLASH ) );
		_qSplashScreen->show();
		_qSplashScreen->showMessage( "Loading..." );

		_loadTheme();

		// TODO: move to parent.
		// Internal::initSettings( App::SETTINGS() );
		// App::Core::init( dynamic_cast<App::Mode::Visualization &>( *_currentMode ) );
		//_currentMode->enter();
		// createMenu<Menu::File>();

		_qApplication->connect( _qApplication, &QApplication::aboutToQuit, [ this ]() { stop(); } );
	}

	void Application::_start()
	{
		try
		{
			_restoreSettings();
		}
		catch ( const std::exception & e )
		{
			VTX_ERROR( "Failed to restore settings: {}", e.what() );
		}

		_qSplashScreen->finish( _mainWindow.get() );
		_mainWindow->show();
		_qApplication->exec();
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
		_qApplication->setStyle( "fusion" );

		QPalette palette = _qApplication->palette();

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

		_qApplication->setPalette( palette );
	}

	void Application::_saveSettings()
	{
		QSettings settings(
			QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat
		);

		VTX_INFO( "Saving settings: {}", settings.fileName().toStdString() );
		settings.setValue( "geometry", _mainWindow->saveGeometry() );
		settings.setValue( "windowState", _mainWindow->saveState() );
		settings.sync();

		if ( settings.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( settings.status() ) ) );
		}
	}

	void Application::_restoreSettings()
	{
		QSettings settings(
			QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat
		);

		if ( settings.status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( settings.status() ) ) );
		}

		VTX_INFO( "Restoring settings: {}", settings.fileName().toStdString() );
		_mainWindow->restoreGeometry( settings.value( "geometry" ).toByteArray() );
		_mainWindow->restoreState( settings.value( "windowState" ).toByteArray() );
	}

} // namespace VTX::UI::QT
